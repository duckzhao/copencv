#include<iostream>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

using namespace std;

int main() {

	string path = "Resources/cards.jpg";
	cv::Mat img = cv::imread(path);

	// 仿射变换，将形状扭曲的矩形图案，映射为正规矩形
	// King
	// 首先定义两个点集，src表示 扭曲图案在原图上的四个坐标点，dst表示映射后的新img上对应的四个坐标点，因为中间要计算变换矩阵，会有除法计算，因此定义为Point2f
	cv::Point2f src[4] = { {529, 142}, {771, 190}, {405, 395}, {674, 457} };
	// 定义目标矩形的大小,考虑到映射也定义为float类型
	float w = 250, h = 350;
	// src表示映射后的新图案四个点坐标，因为我们将新图案重新绘制一个图，所以起始点坐标是 (0，0)
	cv::Point2f dst[4] = { {0.0f,0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };	
	// 求变换矩阵
	cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
	// 开始变换,定义一个存储变换后规则矩阵的mat
	cv::Mat imgWarp;
	cv::warpPerspective(img, imgWarp, matrix, cv::Size(w, h));

	// Queen
	cv::Point2f src2[4] = { {64, 324}, {338, 277}, {87, 635}, {404, 574} };
	cv::Point2f dst2[4] = { {0.0f,0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };
	matrix = cv::getPerspectiveTransform(src2, dst2);
	cv::warpPerspective(img, imgWarp, matrix, cv::Size(w, h));

	// 展示四个角定位情况,可以更快的判断点的调整方式
	for (int i = 0; i < 4; i++)
	{
		// src 是一个 point 数组，里面的元素是point
		cv::circle(img, src[i], 5, cv::Scalar(0, 0, 255), cv::FILLED);
	}

	cv::imshow("imgWarp", imgWarp);
	cv::imshow("img", img);
	cv::waitKey(0);
	return 0;
}