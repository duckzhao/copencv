#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<string>
#include<vector>
#include<iostream>

using namespace std;

void getContours(cv::Mat imgDil, cv::Mat img) {

	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;	// Vec4i表示 这个 是一个size为4的存放int类型数据的 vector 容器

	// 函数文档见：https://blog.csdn.net/laobai1015/article/details/76400725
	// contours 用于存放每一个轮廓的点集坐标，hierarchy 存放用于描述contours中每个轮廓和其他轮廓关系的int变量，CV_RETR_EXTERNAL只检测最外围轮廓，CV_CHAIN_APPROX_SIMPLE 仅保存轮廓的拐点信息
	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 在此顺便定义接收多边形拟合后 点集 的点集容器
	vector<vector<cv::Point>> canPoly(contours.size());

	// 顺便定义接受 轮廓图形 最小外界矩形 bbox 的容器
	vector<cv::Rect> boundRect(contours.size());

	// 该函数执行完毕后，contours 中存放了 imgDil 图像中所有的 最外围轮廓 的 拐点坐标
	// 我们可以 在原图上绘制这些轮廓，即把每个轮廓中的 拐点集 坐标全部连接起来，opencv已经封装好了这个函数，无需我们写循环去自己连接了
	// 第三个参数表示要绘制的轮廓编号，-1表示绘制所有轮廓
	// cv::drawContours(img, contours, -1, cv::Scalar(0, 255, 0), 2);

	// 现在过滤这个小黑点的了轮廓，因为他是个噪声，考虑到他的形状比较小，使用面积来过滤
	// 遍历存放每个轮廓拐点集的容器 contours 中的每个轮廓
	for (int i = 0; i < contours.size(); i++)
	{
		// 计算当前遍历 编号 轮廓的 面积
		int area = cv::contourArea(contours[i]);
		// cout << area << endl;
		// 判断面积大小，当面积大于200时，才绘制他的轮廓	// 注：这里不论计算周长还是面积，都是像素级别上的，和实际尺寸不直接关联
		if (area >= 200)
		{

			// 然后求得这些非噪音轮廓物体的周长
			float peri = cv::arcLength(contours[i], true);

			// 然后使用多变拟合函数，拟合检测到的每个轮廓上的 拐点。作用：主要功能是把一个 连续光滑曲线(拐点集) 折线化，对图像轮廓点进行多边形拟合
			// 需要有一个和contours 类别的容器接收返回的 拟合后的 contour，拟合后的 contours 大小小于等于 原始 contours，我们将其定义在循环体外部
			// https://blog.csdn.net/kakiebu/article/details/79824856
			cv::approxPolyDP(contours[i], canPoly[i], 0.02 * peri, true);
			
			// 通过打印拟合前后 检测到的 轮廓 拐点数可以看出，拟合前点太多了，没法依据这些点判断 闭合轮廓物体 的形状，但拟合后就点很少，基本接近真实拐点分布（多边形）
			cout << canPoly[i].size() << " " << contours[i].size() << endl;
			// 仅绘制当前正在遍历的这个 编号 的轮廓
			// 当填入contours绘制时，被绘制的点很多，连续绘制成一条线，当填入拟合后的canPoly绘制时，每个图案仅会绘制极个别的折线点，这有利于我们后期区分他们的形状
			// cv::drawContours(img, contours, i, cv::Scalar(0, 255, 0), 2);
			cv::drawContours(img, canPoly, i, cv::Scalar(0, 255, 0), 2);

			// 计算这些 轮廓图形的 最小外接矩形，boundingbox
			boundRect[i] = cv::boundingRect(canPoly[i]);
			// 接着绘制这些最小外界矩形
			cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 255), 5);


			// 接着使用 拟合后的较少的 canPoly 折线点的数量 判断当前轮廓的形状
			int objCor = (int)canPoly[i].size();
			string objType;
			if (objCor == 3)
			{
				objType = "Tri";
			}
			else if (objCor == 4)
			{
				// 角点为4，就判断是长方形还是正方形
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				if (aspRatio >= 0.95 && aspRatio <= 1.05)
				{
					objType = "Square";
				}
				else
				{
					objType = "Rect";
				}
			}
			else
			{
				objType = "Circle";
			}
			// 给它外接矩形的左上角，偏移5个像素的位置，写他的类型
			cv::putText(img, objType, { boundRect[i].x ,boundRect[i].y - 5 }, cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(255, 255, 0));
		}
	}

}




int main() {

	// 形状检测，三角形，圆形，正方形等等

	string path = "./Resources/shapes.png";
	cv::Mat img;
	img = cv::imread(path);

	// preprocess
	cv::Mat imgGray, imgBlur, imgCanny, imgDil;
	cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 3, 0);
	cv::Canny(imgBlur, imgCanny, 25, 75);
	// 提取canny边缘之后最好膨胀一下，不然提取的边缘可能会有边角分割，没有完全连在一起，形成一个闭合连通区域
	cv::Mat kernal = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(imgCanny, imgDil, kernal);

	// 将膨胀处理后的canny边缘送入getContours函数，进一步处理，求得轮廓坐标、边角点坐标，面积==
	getContours(imgDil, img);

	cv::imshow("img", img);
	cv::imshow("imgCanny", imgDil);
	cv::waitKey(0);

	return 0;
}