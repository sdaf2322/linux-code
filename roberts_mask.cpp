#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void filter(Mat img, Mat& dst, Mat mask)         // 회선 수행 함수
{
    dst = Mat(img.size(), CV_32F, Scalar(0));      // 회선 결과 저장 행렬
    Point h_m = mask.size() / 2;               // 마스크 중심 좌표

    for (int i = h_m.y; i < img.rows - h_m.y; i++) {   // 입력 행렬 반복 순회
        for (int j = h_m.x; j < img.cols - h_m.x; j++)
        {
            float sum = 0;
            for (int u = 0; u < mask.rows; u++) {   // 마스크 원소 순회
                for (int v = 0; v < mask.cols; v++)
                {
                    int y = i + u - h_m.y;
                    int x = j + v - h_m.x;
                    sum += mask.at<float>(u, v) * img.at<uchar>(y, x); // 회선 수식
                }
            }
            dst.at<float>(i, j) = sum;            // 회선 누적값 출력화소 저장
        }
    }
}

void differential(Mat image, Mat& dst, float data1[], float data2[])
{
    Mat dst1, dst2;
    Mat mask1(3, 3, CV_32F, data1);         // 입력 인수로 마스크 행렬 초기화
    Mat mask2(3, 3, CV_32F, data2);

    filter(image, dst1, mask1);            // 사용자 정의 회선 함수
    filter(image, dst2, mask2);
    magnitude(dst1, dst2, dst);            // 회선 결과 두 행렬의 크기 계산


    dst1 = abs(dst1);                  // 회선 결과 음수 원소를 양수화
    dst2 = abs(dst2);
    dst.convertTo(dst, CV_8U);            // 윈도우 표시 위한 형변환
    dst1.convertTo(dst1, CV_8U);            // 윈도우 표시 위한 형변환
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
    differential(image, dst, data1, data2);   // 두 방향의 회선 수행 및 크기 계산

    imshow("image", image);
    imshow("로버츠 에지", dst);
    waitKey();
    return 0;
}