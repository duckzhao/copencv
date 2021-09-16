#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>

using namespace std;
using namespace cv;

int main() {
	string path = "Resources/test.png";
	Mat img = imread(path);
	imshow("image", img);
	waitKey(0);
	return 0;
}