#include <opencv2/opencv.hpp>

using namespace sd;
using namespace cv;

int main()
{
	Mat image = Mat::zeros(300, 400, CV_8UC3);
	Scalar color(255, 255, 0);
	std::vector<int> jpegParams, pngParams;
	jpegParams.push_back(IMWRITE_JPEG_QUALITY);
	jpegParams.push_back(80);
	jpegParams.push_back(IMWRITE_PNG_COMPRESSION);
	pngParams.push_back(9);

	circle(image, Point(100, 100), 50, color, -1);
	
	imwrite("sample.jpg", image, jpegParams);
	imwrite("sample.png", image, pngParams);

	return 0;
}