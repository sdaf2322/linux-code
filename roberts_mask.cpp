#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void filter(Mat img, Mat& dst, Mat mask)         // ȸ�� ���� �Լ�
{
    dst = Mat(img.size(), CV_32F, Scalar(0));      // ȸ�� ��� ���� ���
    Point h_m = mask.size() / 2;               // ����ũ �߽� ��ǥ

    for (int i = h_m.y; i < img.rows - h_m.y; i++) {   // �Է� ��� �ݺ� ��ȸ
        for (int j = h_m.x; j < img.cols - h_m.x; j++)
        {
            float sum = 0;
            for (int u = 0; u < mask.rows; u++) {   // ����ũ ���� ��ȸ
                for (int v = 0; v < mask.cols; v++)
                {
                    int y = i + u - h_m.y;
                    int x = j + v - h_m.x;
                    sum += mask.at<float>(u, v) * img.at<uchar>(y, x); // ȸ�� ����
                }
            }
            dst.at<float>(i, j) = sum;            // ȸ�� ������ ���ȭ�� ����
        }
    }
}

void differential(Mat image, Mat& dst, float data1[], float data2[])
{
    Mat dst1, dst2;
    Mat mask1(3, 3, CV_32F, data1);         // �Է� �μ��� ����ũ ��� �ʱ�ȭ
    Mat mask2(3, 3, CV_32F, data2);

    filter(image, dst1, mask1);            // ����� ���� ȸ�� �Լ�
    filter(image, dst2, mask2);
    magnitude(dst1, dst2, dst);            // ȸ�� ��� �� ����� ũ�� ���


    dst1 = abs(dst1);                  // ȸ�� ��� ���� ���Ҹ� ���ȭ
    dst2 = abs(dst2);
    dst.convertTo(dst, CV_8U);            // ������ ǥ�� ���� ����ȯ
    dst1.convertTo(dst1, CV_8U);            // ������ ǥ�� ���� ����ȯ
    dst2.convertTo(dst2, CV_8U);
    imshow("dst1", dst1);
    imshow("dst2", dst2);
}

int main()
{
    Mat image = imread("./image1/baby24.bmp", IMREAD_GRAYSCALE);
    CV_Assert(image.data);

    float data1[] = {
       -1, 0, 0,
       0, 1, 0,
       0, 0, 0
    };
    float data2[] = {
       0, 0, -1,
       0, 1, 0,
       0, 0, 0
    };

    Mat dst;
    differential(image, dst, data1, data2);   // �� ������ ȸ�� ���� �� ũ�� ���

    imshow("image", image);
    imshow("�ι��� ����", dst);
    waitKey();
    return 0;
}