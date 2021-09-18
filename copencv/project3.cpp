#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>
#include<vector>

using namespace std;

int main() {

	// 当然这里可以调用摄像头读取视频流，然后写一个循环专门用来做检测每个视频帧 是否有车牌
	// 检测到车牌以后再把车牌送到深度网络里去识别车牌号即可

	string path = "Resources/plate.jpg";
	cv::Mat img = cv::imread(path);

	// 创建检测车牌的检测器
	cv::CascadeClassifier plateCascade;
	plateCascade.load("./Resources/haarcascade_russian_plate_number.xml");

	// 定义存放车牌检测结果的容器
	vector<cv::Rect> plates;

	// 开始检测
	plateCascade.detectMultiScale(img, plates, 1.1, 10);

	cout << plates.size() << "xxxxxxxxxxxxxxxxxxxxxxxx" << endl;

	for (int i = 0; i < plates.size(); i++)
	{
		// 将有车牌号的roi区域截取下来
		cv::Mat imgPlate = img(plates[i]);

		// 并将其保存下来
		cv::imwrite("./Resources/Plates/" + to_string(i) + ".jpg", imgPlate);

		// 绘制矩形在原图上
		cv::rectangle(img, plates[i].tl(), plates[i].br(), cv::Scalar(0, 0, 255), 3);
	}
	
	cv::imshow("image", img);
	cv::waitKey(0);
	return 0;
}