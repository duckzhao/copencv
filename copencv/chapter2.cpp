#include<iostream>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<string>

using namespace std;
using namespace cv;

// c++似乎不常用返回值这个东西，喜欢直接传入一个指针作为参数，然后在函数内就赋值了


void processImg() {
	string path = "./Resources/test.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;
	// 转为灰度
	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	// 高斯模糊，卷积核越大越模糊
	GaussianBlur(img, imgBlur, Size(7, 7), 5, 0);

	// 边缘检测---边缘检测的图像常是模糊过的图像，消除一些梯度明显的噪声
	// 阈值越小，边缘越多，模糊程度越大，边缘越少
	Canny(imgBlur, imgCanny, 25, 75);

	// 膨胀
	// 膨胀的第三个参数是一个array kernal，要使用Mat格式定义，指定一个3*3的矩形核
	// 膨胀核越大线条越粗
	Mat kernal = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernal);
	// 腐蚀核越大线条越少
	erode(imgDil, imgErode, kernal);

	/*imshow("img", img);
	imshow("imgGray", imgGray);
	imshow("imgBlur", imgBlur);*/
	imshow("imgCanny", imgCanny);
	imshow("imgDil", imgDil);
	imshow("imgErode", imgErode);
	waitKey(0);
}


int main() {
	processImg();
	return 0;
}