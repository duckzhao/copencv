#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

// 处理后a4纸的大小
float width = 420;
float height = 596;
int cropSize = 5;

// 预处理，得到待处理图片的边缘检测结果
cv::Mat preProcess(cv::Mat img) {
	// 灰度
	cv::Mat imgGray;
	cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
	// 模糊
	cv::Mat imgBlur;
	cv::GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 3, 0);
	// 边缘检测
	cv::Mat imgCanny;
	cv::Canny(imgBlur, imgCanny, 25, 75);
	// 膨胀
	cv::Mat imgDil;
	cv::Mat kernal = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(imgCanny, imgDil, kernal);

	// 膨胀后的边缘更加连续，便于检测轮廓
	return imgDil;
}

// 获得最大面积的矩形轮廓（即a4纸）的四个像素点坐标
vector<cv::Point> getBiggestContour(cv::Mat imgCanny, cv::Mat imgOriginal) {

	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(imgCanny, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 多边形拟合的轮廓存储下来
	vector<vector<cv::Point>> canPoly(contours.size());

	// 存储最大矩形目标结果的坐标点
	vector<cv::Point> biggestPoint;
	// 遍历寻找最大面积的轮廓
	float max_area = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		// 这里不能加另一个bool参数为true
		float area = cv::contourArea(contours[i]);
		// cout << area << endl;
		// 仅对面积大于500的闭合轮廓进行判断
		if (area > 500)
		{
			float peri = cv::arcLength(contours[i], true);
			// 多边形拟合
			cv::approxPolyDP(contours[i], canPoly[i], 0.02 * peri, true);
			// 目标的面积必须大于当前最大的，且拟合后的结果是矩形才能入选 ocr 目标
			if (area > max_area && canPoly[i].size() == 4)
			{
				max_area = area;
				// 绘制一下多边形拟合后的轮廓结果
				cv::drawContours(imgOriginal, canPoly, i, cv::Scalar(255, 0, 255), 3);
				biggestPoint = { canPoly[i][0], canPoly[i][1], canPoly[i][2], canPoly[i][3] };
			}
		}	
	}
	// 如果没有检测到则size是0
	// 检测到的原始多边形轮廓坐标是无序的，需要手动对四个角坐标进行排序，排序目的是和透视变换那边对应起来
	vector<cv::Point> biggestPointReal(biggestPoint.size());
	if (biggestPoint.size() != 0)
	{
		float max_point = 0;
		int max_index;
		float min_point = 100000;
		int min_index;
		for (int i = 0; i < biggestPoint.size(); i++)
		{
			// 首先找 编号3 的点，x+y 最大
			if (biggestPoint[i].x + biggestPoint[i].y > max_point)
			{
				biggestPointReal[3] = { biggestPoint[i].x, biggestPoint[i].y };
				max_point = biggestPoint[i].x + biggestPoint[i].y;
				max_index = i;
			}
			// 再找 编号0 的点， x+y 最小
			if (biggestPoint[i].x + biggestPoint[i].y < min_point)
			{
				biggestPointReal[0] = { biggestPoint[i].x, biggestPoint[i].y };
				min_point = biggestPoint[i].x + biggestPoint[i].y;
				min_index = i;
			}
		}
		// 找到剩余两个未被筛选的点
		vector<cv::Point> tempPointReal;
		for (int i = 0; i < biggestPoint.size(); i++)
		{
			// 排除是已经选出的0，3点
			if ((biggestPoint[i].x == biggestPointReal[0].x && biggestPoint[i].y == biggestPointReal[0].y) or (biggestPoint[i].x == biggestPointReal[3].x && biggestPoint[i].y == biggestPointReal[3].y))
			{
				continue;
			}
			else
			{
				
				tempPointReal.push_back(biggestPoint[i]);
			}
		}

		// 找编号为 1 的点，x比较大，y比较小
		if (tempPointReal[0].x > tempPointReal[1].x)
		{
			biggestPointReal[1] = { tempPointReal[0].x, tempPointReal[0].y };
			biggestPointReal[2] = tempPointReal[1];
		}
		else
		{
			biggestPointReal[1] = tempPointReal[1];
			biggestPointReal[2] = tempPointReal[0];
		}

	}
	return biggestPointReal;
}

// 绘制最大纸张的四个坐标点圈圈，用于判断获取到准不准
void drawPoints(vector<cv::Point> points, cv::Mat imgOriginal) {
	if (points.size() == 0)
	{
		return;
	}

	for (int i = 0; i < points.size(); i++)
	{
		cv::circle(imgOriginal, points[i], 5, cv::Scalar(0, 255, 0), cv::FILLED);
		cv::putText(imgOriginal, to_string(i), cv::Point(points[i].x, points[i].y - 5), cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(0, 0, 255), 4);
	}
}

// 完成透视变换，截取a4纸内容为单独的一张 矩形 图片
cv::Mat processImgWarp(cv::Mat img, vector<cv::Point> biggestPoints) {
	// 定义原图坐标
	cv::Point2f src[4] = {biggestPoints[0], biggestPoints[1], biggestPoints[2], biggestPoints[3]};
	// 变换后的新图坐标
	cv::Point2f dst[4] = { {0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}, {width, height} };

	// 求变换矩阵
	cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
	// 开始变换,定义一个存储变换后规则矩阵的mat
	cv::Mat imgWarp;
	cv::warpPerspective(img, imgWarp, matrix, cv::Size(width, height));
	return imgWarp;
}


int main() {
	string path = "Resources/paper.jpg";
	cv::Mat img = cv::imread(path);
	cv::Mat imgOriginal;
	// 图像太大了，缩小比例为一半
	// cv::resize(img, img, cv::Size(), 0.5, 0.5);
	img.copyTo(imgOriginal);	// 保留img作为副本

	// preprocess
	cv::Mat imgCanny;
	imgCanny = preProcess(imgOriginal);

	// get biggest contour
	vector<cv::Point> biggestPoints = getBiggestContour(imgCanny, imgOriginal);
	drawPoints(biggestPoints, imgOriginal);
	
	// warp
	cv::Mat imgWarp;
	imgWarp = processImgWarp(img, biggestPoints);

	// crop，修剪黑边
	cv::Mat imgCrop;
	// roi(x1, x1, w, h);
	cv::Rect roi(cropSize+5, cropSize+5, (int)(width - cropSize*2), (int)(height - cropSize*2));
	imgCrop = imgWarp(roi);

	cv::imshow("imgOriginal", imgOriginal);
	// cv::imshow("image canny", imgCanny);
	cv::imshow("image wrap", imgCrop);
	// cv::imshow("img", img);
	cv::waitKey(0);
	return 0;
}