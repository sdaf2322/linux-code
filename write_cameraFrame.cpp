#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	VideoCapture capture(0);
	CV_Assert(capture.isOpened());

	double fps = 29.97;
	int	delay = cvRound(1000.0 / fps);
	Size size(640, 360);
	int fourcc = VideoWriter::fourcc('D', 'X', '5', '0');

	capture.set(CAP_PROP_WIDTH, size.width);
	capture.set(CAP_PROP_FRAME_HEIGHT, size.height);

	cout << "width x height : " << size << endl;
	cout << "VideoWriter::fource : " << fourcc << endl;
	cout << "display : " << delay << endl;
	cout << "fps : " << fps << endl;

	VideoWriter writer;
	writer.open("../image/video_file.avi", fourcc, fps, size);
	CV_Assert(writer.isOpened());

	for (;;) {
		Mat frame;
		capture >> frame;
		writer << frame;

		imshow("ī�޶� ���󺸱�", frame);
		if (waitKey(delay) >= 0)
			break;
	}
	return 0;
	
}