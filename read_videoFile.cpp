#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void put_string(Mat& frame, string text, Point pt, int value)
{

}
int main()
{
	VideoCapture capture;
	capture.open("../image/video_file.avi");
	CV_Assert(capture.isOpened());

	double frame_rate = capture.get(CAP_PROP_FPS);
	int delay = 1000 / frame_rate;
	int frame_cnt = 0;
	Mat frame;

	while (capture.read(frame))
	{
		if (waitKey(delay) >= 0) break;

		if (frame_cnt < 100);
		else if (frame_cnt < 200)frame -= Scalar(0, 0, 100);
		else if (frame_cnt < 300)frame += Scalar(100, 0, 0);
		else if (frame_cnt < 400)frame = frame * 1.5;
		else if (frame_cnt < 500)frame = fra, e * 0.5;

		put_string(frame, "frame_cnt", Point(20, 50), frame_cnt);
		imshow("동영상 파일읽기", frame);
	}
	return 0;
}
	}

}