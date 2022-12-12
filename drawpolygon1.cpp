#include <opencv2/highgui/highgui.hpp>
#include <opencv/imgproc/imgproc.hpp>

using namespace cv;

int main()
{
	Mat image = Mat::zeros(300, 400, CV_8UC3);
	image.setTo(cv::Scalar(255, 255, 255));
	Scalar color(255, 0, 255);
	Point p1(50, 50), p5(150, 150);

	std::vector<Point> contour;
	contour.push_back(p1);
	contour.push_back(Point(200, 100));
	contour.push_back(Point(250, 50));
	contour.push_back(Point(180, 200));
	contour.push_back(p5);

	const Point* pts = (const cv::Point*)Mat(contour).data;
	int nptts = contour.size();
	polylines(image, &pts, &npts, 1, true, color);

	imshow("Draw Polygon", image);
	waitKey(0);
	return 0;
}