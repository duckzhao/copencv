#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
#include<string>

using namespace std;
using namespace cv;

// 实际上感觉c++的opencv用法和python中的基本一致，所以可以类比这两个的api进行学习


/// <summary>
/// 
/// </summary>
/// <returns></returns>
// 读取一张图片，展示，并返回numpy矩阵
Mat read_img() {
	string path = "Resources/test.png";
	Mat img = imread(path);
	imshow("image", img);
	waitKey(0);
	return img;
}


// 读取一个本地视频，并且展示
void read_video() {
	// 实例化一个VideoCapture对象
	VideoCapture cap("./Resources/test_video.mp4");
	Mat frame;
	while (true)
	{
		// cap.read 直接把接收当前帧的变量，作为参数传给read函数即可
		cap.read(frame);
		if (frame.dims == 0)
		{
			break;
		}
		imshow("video", frame);
		cv::waitKey(15);

	}
}

// 启用本地摄像头读取视频
void read_capture() {
	// 0表示本地第0个摄像头
	VideoCapture cap(0);
	Mat frame;
	int key_code;
	while (true)
	{
		cap.read(frame);
		cv::imshow("capture", frame);
		key_code = waitKey(1);
		// 27是 esc键的键码
		if (key_code == 27)
		{
			break;
		}
	}
	cap.release();
	destroyAllWindows();
}

int main() {
	// read_img();
	// read_video();
	read_capture();
}