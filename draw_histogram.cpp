#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256)
{
    hist = Mat(bins, 1, CV_32F, Scalar(0));
    float gap = range_max / (float)bins;

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int idx = int(image.at<uchar>(i, j) / gap);
            hist.at<float>(idx)++;
        }
    }
}

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

int main()
{
    Mat image = imread("../image1/baby24.bmp", IMREAD_GRAYSCALE);
    CV_Assert(!image.empty());

    Mat hist, hist_img;

    calc_Histo(image, hist, 256);
    draw_histo(hist, hist_img);

    imshow("image", image);
    imshow("hist_img", hist_img);
    waitKey();

    return 0;
}
/*
int main()
{
   Mat image = imread("./lena.bmp", 1);
   CV_Assert(!image.empty());

   Mat HSV_img, HSV_arr[3];
   cvtColor(image, HSV_img, BGR2HSV);
   split(HSV_img, HSV_arr);

   Mat hue_hist, hue_hist_img;
   calc_Histo(HSV_arr[0], hue_hist, 18, 180);
   draw_histo(hue_hist, hue_hist_img, Size(360, 200));

   imshow("image", image);
   imshow("Hue_hist_img", hue_hist_img);
   waitKey();

   return 0;
}*/