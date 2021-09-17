#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<string>
#include<iostream>
#include <typeinfo> 

using namespace std;

void main() {
	string path = "./Resources/test.png";
	cv::Mat img = cv::imread(path);

	// 查看图片size
	// img.size(),返回的是cv::size类型，能够直接输出应该是重载了<<运算符
	cout << typeid(img.size()).name() << endl;
	cout << img.size() << endl;
	cv::imshow("img", img);

	// resize图片
	cv::Mat imgResize;
	// 第一种方式 传入cv::Size(500, 500)，目标size
	cv::resize(img, imgResize, cv::Size(500, 500));
	// 第二种方式，指定长宽缩放比例---c++没有使用形参名赋值的方式，且不可以跳过函数参数
	cv::resize(img, imgResize, cv::Size(), 0.5, 0.5);
	cv::imshow("imgResize", imgResize);

	// crop图片 cv::Rect roi(x1, y1, x2, y2);
	cv::Rect roi(100, 100, 500, 200);
	cv::Mat imgCrop;
	// 直接给img里面传一个Rect矩阵，表示剪裁的区域即可，剪裁只管xy坐标，不管通道数
	imgCrop = img(roi);

	cv::imshow("imgCrop", imgCrop);

	cv::waitKey(0);
}