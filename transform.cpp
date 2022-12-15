#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	vector<Point> rect_pt1, rect_pt2;													// 4개의 좌표를 지정할 벡터
	rect_pt1.push_back(Point(200, 50));
	rect_pt1.push_back(Point(400, 50));
	rect_pt1.push_back(Point(400, 250));
	rect_pt1.push_back(Point(200, 250));

	float theta = 20 * CV_PI / 180;														// 
	Matx22f m(cos(theta), sin(theta), sin(theta), cos(theta));							// 2행 2열 행렬 만들기
	transform(rect_pt1, rect_pt2, m);													// 

	Mat image(400, 500, CV_8UC3, Scalar(255, 255, 255));								// 400x500 크기이고 채널이 3개인 백색 이미지 생성
	for (int i = 0; i < 4; i++)
	{
		line(image, rect_pt1[i], rect_pt1[(i + 1) % 4], Scalar(0, 0, 0), 1);			// 원본 이미지(사각형)을 검은색 선으로 출력
		line(image, rect_pt2[i], rect_pt2[(i + 1) % 4], Scalar(255, 0, 0), 2);			// 돌려진 이미지를 파란색 서능로 출력
		cout << "rect_pt1[" + to_string(i) + "]=" << rect_pt1[i] << "\t";
		cout << "rect_pt2[" + to_string(i) + "]=" << rect_pt2[i] << endl;
	}

	imshow("image", image);
	waitKey();
	return 0;
}