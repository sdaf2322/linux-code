#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"


inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)                                     // 색상 값이 255가 넘으면 해당 색상을 255로, 색상 값이 0보다 작으면 0으로 설정
{
    return(value > max ? max : value < min ? min : value);
}

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)
typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             
    BITMAPINFOHEADER bmpInfoHeader;     
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg, * padding;
    int x, y, z, imageSize, pimageSize;

    if(argc != 3) {                                                               // 인자의 개수가 3개가 아니면 에러
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) {                                       // 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                           // 파일에서 파일헤더 읽어서 구조체에 저장  

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                       // 파일에서 인포헤더 읽어서 구조체에 저장

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {                                          // 24비트 지원안하면 실행안함
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;                                    // rgb 개수, 3개
    int size = bmpInfoHeader.biWidth*elemSize;                                    // 열의 개수
    int psize = (bmpInfoHeader.biWidth + 2) * elemSize;                           // 패딩의 열의 개수
    imageSize = size * bmpInfoHeader.biHeight;                                    // 행열의 개수
    pimageSize = imageSize + (((bmpInfoHeader.biWidth + bmpInfoHeader.biHeight + 2) * 2) * 3);      // 패딩의 행열 개수

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);                // 해상도 출력
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);                                           // 픽셀당 비트수 출력
    printf("Image Size : %d\n", imageSize);                                                         // 행열의 개수 출력
    printf("Padding Size : %d\n", pimageSize);                                                      // 패딩의 행열 개수 출력

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                                // 원본 이미지를 저장할 메모리 할당
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                               // 변경된 이미지 저장할 메모리 할당
    padding = (ubyte*)malloc(sizeof(ubyte)*pimageSize);                                             // 패딩 메모리 할당
    fread(inimg, sizeof(ubyte), imageSize, fp);                                                     // 파일에서 원본 이미지 읽음
    
    fclose(fp);                                                                                     // 파일 닫기

    // define the kernel
    float kernel[3][3] = { {0.0, -1.0, 0},                                                          // 마스크 정의, 이미지 sharp 
                           {-1.0, 5.0, -1.0},
                           {0.0, -1.0, 0.0} };
    memset(padding, 0, sizeof(ubyte)*pimageSize);                                                   // 패딩 0으로 초기화

    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              // 패딩 정의, 사이드는 0으로 저장, 사이드를 제외한 부분은 원본 이미지로 저장
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                padding[x + y * psize + z] = inimg[(x - elemSize) + (y - 1) * size + z];              
            }
        }
    }

    for (y = 0; y < bmpInfoHeader.biHeight + 2; y++) {                                              // 사이드를 인접한 이미지의 색상 값으로 설정
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                if (y == 0 || y == bmpInfoHeader.biHeight + 1)
                    padding[x + y * psize + z] = inimg[(x - elemSize) + y * size + z];
            }
        }
    }

    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              // 사이드를 인접한 이미지의 색상 값으로 설정
        for (x = 0; x < (bmpInfoHeader.biWidth + 2) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                if (x == 0 || x == bmpInfoHeader.biWidth + 1)
                    padding[x + y * psize + z] = inimg[x + (y - 1) * size + z];
            }
        }
    }

    for (y = 1; y < bmpInfoHeader.biHeight + 1; y++) {                                              // 패딩 이미지와 마스킹을 곱해서 outimg로 출력
        for (x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x += elemSize) {
            for (z = 0; z < elemSize; z++) {
                float sum = 0.0;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                            sum += kernel[i + 1][j + 1] * padding[(x + i * elemSize) + (y + j) * psize + z];
                    }
                }
                outimg[x - 3 + (y - 1) * size + z] = clip(sum, 0, 255);
            }
        }
    }
       
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) {                                                           // 출력 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                            // 파일헤더 정보 파일에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                        // 인포헤더 정보 파일에 저장

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);                                                   // 출력 이미지 파일에 저장

    fclose(fp);                                                                                     // 파일 닫기
    
    free(inimg); 
    free(outimg);                                                                       
    
    return 0;
}
