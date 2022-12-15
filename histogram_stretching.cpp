#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void calc_Histo(const Mat& image, Mat& hist, int bins)
void draw_histo(Mat hist, Mat& hist_img, Size size = Size(256, 200))
{
    hist_img = Mat(size, CV_8U, Scalar(255));
    float bin = (float)hist_img.cols / hist.rows;
    normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX);

    for (int i = 0; i < hist.rows; i++) {
        float start_x = i * bin;
        float end_x = (i + 1) * bin;
        Point2f pt1(start_x, 0);
        Point2f pt2(end_x, hist.at<float>(i));

        if (pt2.y > 0)
            rectangle(hist_img, pt1, pt2, Scalar(0), -1);

    }
    flip(hist_img, hist_img, 0);
}

int search_valueIdx(Mat hist, int bias = 0)
{
    for (int i = 0; i < hist.rows; i++)
    {
        int idx = abs(bias - i);
        if (hist.at<float>(idx) > 0)
            return idx;
    }
    return -1;
}

int main()
{
    Mat image = imread("../image1/baby24.bmp", 0);
    CV_Assert(!image.empty());

    Mat hist, hist_dst, hist_img, hist_dst_img;
    int histsize = 64, ranges = 256;
    calc_histo(image, hist, histsize, ranges);

    float bin_width = (float)ranges / histsize;
    int low_value
}