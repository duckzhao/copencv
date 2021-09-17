#include<iostream>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
using namespace std;


/*
色调（H），饱和度（S），亮度（V）, H:  0— 180, S:  0— 255, V:  0— 255
*/

// 已知映射后hsv色域 指定颜色的筛选区间
void inRangeHsvColor() {
	// hsv色域的筛选区间
	int hmin = 0, smin = 110, vmin = 153;
	int hmax = 19, smax = 240, vmax = 255;
	// 图片颜色筛选一般思路：①BGR区域转换到HSV区域；②在hsv色域对映射后的图片颜色进行hsv过滤；③得到过滤后颜色区域的掩码图，用于在原图上筛选指定原始颜色区域

	string path = "Resources/lambo.png";
	cv::Mat img = cv::imread(path);
	cv::imshow("img", img);

	// hsv空间转换，以便于筛选颜色区间,因为hsv色域空间是连续的，转换后的图片颜色会比较奇怪
	cv::Mat imgHsv;
	cv::cvtColor(img, imgHsv, cv::COLOR_BGR2HSV);

	// 筛选颜色
	cv::Mat mask;	// 用于保存 通过颜色筛选区域的蒙版，
	// 定义颜色筛选区域的上下限---在 imgHsv 图片上对要保留的当前颜色进行区间限定
	cv::Scalar lower(hmin, smin, vmin);
	cv::Scalar upper(hmax, smax, vmax);
	// 通过颜色筛选的区域为白色掩码，否则为黑色掩码，这样就可以在原图上筛选出对应颜色的区域了
	cv::inRange(imgHsv, lower, upper, mask);

	cv::imshow("imgHsv", imgHsv);
	cv::imshow("mask", mask);

	// 筛选完了之后使用原图和mask作 与 运算，在原图上取出 掩码中为白色区域的内容
	cv::Mat imgCrop;
	cv::bitwise_and(img, img, imgCrop, mask);
	cv::imshow("imgCrop", imgCrop);
	cv::waitKey(0);
}


// 往往hsv色域的颜色筛选区间上下限是最难确定的，很难调，因此我们一般使用可以动态拖动的trackbars时时拖动，快速定位某种颜色的hsv上下限
void selectBestHsvLimitValue(string path) {

	int hmin = 0, smin = 0, vmin = 0;
	int hmax = 180, smax = 255, vmax = 255;
	cv::Mat img = cv::imread(path);
	// cv::imshow("img", img);

	// hsv空间转换，以便于筛选颜色区间,因为hsv色域空间是连续的，转换后的图片颜色会比较奇怪
	cv::Mat imgHsv;
	cv::cvtColor(img, imgHsv, cv::COLOR_BGR2HSV);

	cv::Mat mask;	// 用于保存 通过颜色筛选区域的蒙版，


	// 定义一个新窗口，用于嵌入 hsv 值的拖动bar，一个完全独立出来的窗口
	// 第一个参数为窗口名，用于绑定拖动bar，第二个参数是窗口大小size
	cv::namedWindow("Trackbars", (300, 600));
	// 第一个参数为 拖动框的名字，第二个参数为拖动框依附的父窗口名，第三个参数为该拖动框控制的值（传一个int的引用进来），第四个参数为拖动框取值上限
	cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
	cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
	cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
	cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);

	// 在for循环里利用拖动栏更新hsv取值，然后刷新展示，从而快速判断最合适的值
	while (true)
	{
		// 定义颜色筛选区域的上下限---在 imgHsv 图片上对要保留的当前颜色进行区间限定
		cv::Scalar lower(hmin, smin, vmin);
		cv::Scalar upper(hmax, smax, vmax);
		// 筛选颜色
		// 通过颜色筛选的区域为白色掩码，否则为黑色掩码，这样就可以在原图上筛选出对应颜色的区域了
		cv::inRange(imgHsv, lower, upper, mask);

		cv::imshow("imgHsv", imgHsv);
		cv::imshow("mask", mask);
		int keyCode = cv::waitKey(1);
		if (keyCode == 27)
		{
			break;
		}
	}
	
}

int main() {

	inRangeHsvColor();
	// selectBestHsvLimitValue("Resources/shapes.png");
	return 0;
}