#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"


inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)                                             // clip 함수, 값이 255이상이면 255로, 0보다 작으면 0으로 설정
{
    return(value > max ? max : value < min ? min : value);
}

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int static compare(const void* first, const void* second)                                   // 값의 대소비교를 하는 함수
{
    if (*(int*)first > *(int*)second)
        return 1;
    else if (*(int*)first < *(int*)second)
        return -1;
    else
        return 0;
}


int main(int argc, char** argv)
{
    FILE* fp;
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD* palrgb;
    ubyte* inimg, * outimg, * padding;
    int x, y, z, imageSize, pimageSize;

    if (argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }

    /***** read bmp *****/
    if ((fp = fopen(argv[1], "rb")) == NULL) {                                                      // 원본 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n");
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                             // 파일에서 파일헤더 읽어서 구조체에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                         // 파일에서 인포헤더 읽어서 구조체에 저장

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if (bmpInfoHeader.biBitCount != 24) {                                                           // 24비트 지원안하면 실행 안함
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }

    int elemSize = bmpInfoHeader.biBitCount / 8;                                                    // rgb 개수
    int size = bmpInfoHeader.biWidth * elemSize;                                                    // 열의 개수
    int psize = (bmpInfoHeader.biWidth + 2) * elemSize;                                             // 패딩의 열의 개수
    imageSize = size * bmpInfoHeader.biHeight;                                                      // 행열의 개수
    pimageSize = imageSize + (((bmpInfoHeader.biWidth + bmpInfoHeader.biHeight + 2) * 2) * 3);      // 패딩의 행열의 개수

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);
    printf("Padding Size : %d\n", pimageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte) * imageSize);                                              // 원본 이미지를 저장할 메모리 할당
    outimg = (ubyte*)malloc(sizeof(ubyte) * imageSize);                                             // 출력 이미지를 저장할 메모리 할당
    padding = (ubyte*)malloc(sizeof(ubyte) * pimageSize);                                           // 패딩 이미지의 메모리 할당
    fread(inimg, sizeof(ubyte), imageSize, fp);                                                     // 파일에서 원본 이미지 읽어서 inimg에 저장

    fclose(fp);                                                                                     // 파일 닫기
        
    int arr[9];                                                                                     // 이미지를 quick sort하기 위한 배열

    memset(padding, 0, sizeof(ubyte) * pimageSize);                                                 // 패딩 0으로 초기화
    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              // 패딩 정의, 사이드는 0으로, 사이드를 제외한 부분은 원본 이미지로 채움
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                padding[x + y * psize + z] = inimg[(x - elemSize) + (y - 1) * size + z];
            }
        }
    }

    for (y = 0; y < bmpInfoHeader.biHeight + 2; y++) {                                              // 이미지 경계부분 처리
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                if (y == 0 || y == bmpInfoHeader.biHeight + 1)
                    padding[x + y * psize + z] = inimg[(x - elemSize) + y * size + z];
            }
        }
    }

    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              // 이미지 경계부분 처리
        for (x = 0; x < (bmpInfoHeader.biWidth + 2) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                if (x == 0 || x == bmpInfoHeader.biWidth + 1)
                    padding[x + y * psize + z] = inimg[x + (y - 1) * size + z];
            }
        }
    }

        
    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        arr[(i+1)+(j+1)*3] = padding[(x + i * elemSize) + (y + j) * psize + z];     // 지정한 영역의 원본 이미지 값을 배열에 저장
                    }
                }
                qsort(arr, 9, sizeof(int), compare);                                                // 배열의 값들을 quick sort
                outimg[x - 3 + (y - 1) * size + z] = arr[0];                                        // 출력 이미지를 배열의 최대 값으로 지정
            }
        }
    }

    /***** write bmp *****/
    if ((fp = fopen(argv[2], "wb")) == NULL) {                                                      // 쓰기용 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n");
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                            // 파일헤더 파일에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                        // 인포헤더 파일에 저장

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);                                                   // 출력 이미지 파일에 저장

    fclose(fp);                                                                                     // 파일 닫기

    free(inimg);
    free(outimg);

    return 0;
}
