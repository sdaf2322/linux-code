#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat image = imread("../image/bit_test.jpg", IMREAD_COLOR);
	Mat logo = imread("../image/logo.jpg", IMREAD_COLOR);
	Mat logo_th, mask[5], background, foreground, dst;
	CV_Assert(image.data && logo.data);

	threshold(logo, logo_th, 70, 255, THRESH_BINARY);
	split(logo_th, masks);

	bitwise_or(mask[0], mask[1], mask[3]);
	bitwise_or(mask[2], mask[3], mask[3]);
	bitwise_not(mask[3], mask[4]);

	Point center1 = image.size() / 2;
	Point center2 = logo.size() / 2;
	Point start = center1 - center2;
	Rect roi(start, logo.size());

	bitwise_and(logo, logo, foreground, mask[3]);
	bitwise_and(image(roi), image(roi), background, masks4]);

	add(background, foreground, dst)
}