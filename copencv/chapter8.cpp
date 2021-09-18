#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<string>
#include<iostream>
#include<vector>

using namespace std;

int main() {

	string path = "./Resources/test.png";
	cv::Mat img = cv::imread(path);

	// 检测人脸，haar级联方式检测
	cv::CascadeClassifier faceCascade;
	faceCascade.load("./Resources/haarcascade_frontalface_default.xml");
	// 创建一个容器，保存 检测到的face的Rect
	vector<cv::Rect> faces;
	faceCascade.detectMultiScale(img, faces, 1.1, 10);
	
	// 将检测的结果绘制出来
	for (int i = 0; i < faces.size(); i++)
	{
		int x = faces[i].x;
		int y = faces[i].y;
		int w = faces[i].width;
		int h = faces[i].height;
		// cv::rectangle(img, cv::Point(x, y), cv::Point(x + w, y + h), cv::Scalar(0, 255, 255), 3);
		// cv::Rect 中有 tl() 和 br() 方法专门用于返回 左上，右下 坐标
		cv::rectangle(img, faces[i].tl(), faces[i].br(), cv::Scalar(0, 255, 255), 3);
	}

	cv::imshow("img", img);
	cv::waitKey(0);
	return 0;
}