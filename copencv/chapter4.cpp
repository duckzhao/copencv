#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

#include<iostream>

using namespace std;

int main() {

	// 创建一个空白（纯色背景板图片），重载Mat的构造函数
	// CV_8UC3,8位无符号整数3通道图片,c++自带的数据类型，不用加cv库前缀
	cv::Mat img(512, 512, CV_8UC3, cv::Scalar(255, 255, 255));

	// 拷贝一份新的图片img2出来
	cv::Mat img2;
	img.copyTo(img2);

	// 画一个圆,直接在img上面画,线条厚度 写 cv::FILLED 表示填充内部
	cv::circle(img, cv::Point(127, 127), 100, cv::Scalar(0, 255, 0), 3, 6);

	// 画一个矩形框
	cv::rectangle(img, cv::Point(200, 200), cv::Point(400, 400), cv::Scalar(255, 0, 0), cv::FILLED);
	
	// 画线
	cv::line(img, cv::Point(100, 100), cv::Point(300, 300), cv::Scalar(0, 0, 255), 3);

	// 写文字
	cv::putText(img, "zhaokun test", cv::Point(100, 100), cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(0, 0, 0), 2);

	cv::imshow("img", img);
	cv::waitKey(0);
}