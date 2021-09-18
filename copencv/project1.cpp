#include<iostream>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<vector>

using namespace std;

// 定义全局变量，用于存储落笔点,{{x,y,color_index},}      0-purple,1-green
vector<vector<int>> allPoints;

// 存放是的要 过滤的颜色上下限，一个子集一个颜色，存放顺序是 hsvmin-hsvmax 6个取值
vector<vector<int>> myColors = { {68, 203, 59, 132, 255 ,174},
								{124, 48, 117, 143, 170, 255}, // purple
								{68, 72, 156, 102, 126, 255} };	//green

// 上方存的是hsv色域的值，但是我们绘制的时候要用BGR去绘制，因此映射一下
vector<cv::Scalar> myColorsValues = { {255, 0, 255},	// purple
										{0, 255, 0} };	//green

// 寻找某种颜色的笔尖 轮廓外接矩形，并求得笔尖坐标，返回
cv::Point getContours(cv::Mat mask, cv::Mat img) {
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;	// Vec4i表示 这个 是一个size为4的存放int类型数据的 vector 容器

	// 函数文档见：https://blog.csdn.net/laobai1015/article/details/76400725
	// contours 用于存放每一个轮廓的点集坐标，hierarchy 存放用于描述contours中每个轮廓和其他轮廓关系的int变量，CV_RETR_EXTERNAL只检测最外围轮廓，CV_CHAIN_APPROX_SIMPLE 仅保存轮廓的拐点信息
	cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 在此顺便定义接收多边形拟合后 点集 的点集容器
	vector<vector<cv::Point>> canPoly(contours.size());

	// 顺便定义接受 轮廓图形 最小外界矩形 bbox 的容器
	vector<cv::Rect> boundRect(contours.size());

	// 定义一个 画笔 触碰的点坐标
	cv::Point myPoint(0, 0);

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
			cv::Rect boundingbox = cv::boundingRect(canPoly[i]);
			cv::rectangle(img, boundingbox.tl(), boundingbox.br(), cv::Scalar(255, 0, 255), 2);

			// 给myPoint赋值
			myPoint.x = boundingbox.x + boundingbox.width / 2;
			myPoint.y = boundingbox.y;
		}
	}
	// 如果啥都目标颜色轮廓都没检测到，则myPoint返回(0, 0)
	return myPoint;
}



// 在hsv色域定位某种颜色笔尖的运动轨迹
void findColor(cv::Mat img) {
	cv::Mat imgHsv;
	cv::cvtColor(img, imgHsv, cv::COLOR_BGR2HSV);
	
	// 使用一个循环，遍历过滤 myColors 中的 每个颜色
	for (int i = 0; i < myColors.size(); i++)
	{
		cv::Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		cv::Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		cv::Mat mask;
		cv::inRange(imgHsv, lower, upper, mask);
		// cv::imshow(to_string(i), mask);
		
		// 接着对检测每种 颜色笔 的轮廓，获得精准的 落笔点，根据当前循环的i确定具体检测的是什么颜色笔的落笔点，使用一个point对象接收 检测到的点
		// 之前是在canny边缘检测后的图像上找，现在是在二值化的mask上找
		cv::Point tempPoint = getContours(mask, img);

		// 如果啥都没检测到，返回默认 0，0点，则不保存
		if (tempPoint.x != 0 || tempPoint.y != 0)
		{
			// 累加存储（从开始到现在）检测到的每个落笔点(分颜色，用i判断颜色)，用于绘制连续曲线
			allPoints.push_back({ tempPoint.x, tempPoint.y, i });
		}
		
	}
	
}


// 绘制笔尖canvas轨迹
void drawOnCanvas(cv::Mat img) {
	for (int i = 0; i < allPoints.size(); i++)
	{
		// 给每个笔尖点画一个实心圆
		cv::circle(img, cv::Point(allPoints[i][0], allPoints[i][1]), 5, myColorsValues[allPoints[i][2]], cv::FILLED);
	}
}



int main() {
	// 0表示本地第0个摄像头
	cv::VideoCapture cap(0);
	cv::Mat frame;
	int key_code;
	while (true)
	{
		cap.read(frame);
		cv::flip(frame, frame, 1);
		findColor(frame);
		drawOnCanvas(frame);
		cv::imshow("capture", frame);
		key_code = cv::waitKey(1);
		// 27是 esc键的键码
		if (key_code == 27)
		{
			break;
		}
	}
	cap.release();
	cv::destroyAllWindows();
}