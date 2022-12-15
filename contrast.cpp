#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat image = imread("../image1/baby24.bmp", 0);
	CV_Assert(image.data);

	Scalar avg = mean(image) / 2.0;
	Mat dst1 = image * 0.5;
	Mat dst2 = image * 2.0;
	Mat dst3 = image * 0.5 + avg[0];
	Mat dst4 = image * 2.0 - avg[0];

	imshow("image", image);
	imshow("dst1-��񰨼�", dst1), imshow("dst2-�������", dst2);
	imshow("dst3-����̿� ��񰨼�", dst3), imshow("dst4-����̿� �������", dst4);
	waitKey();
	return 0;
}