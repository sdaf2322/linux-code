#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;      
    BITMAPINFOHEADER bmpInfoHeader;     
    RGBQUAD *palrgb;
    ubyte *inimg, *grayimg, *padimg, *outimg;
    int x, y, z, imageWidth, imageSize, graySize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...\n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    imageWidth = bmpInfoHeader.biWidth;
    int elemSize = bmpInfoHeader.biBitCount/8;
    int size = imageWidth*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", imageWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    fclose(fp);

    graySize = sizeof(ubyte)*imageWidth*bmpInfoHeader.biHeight;                                      // 해상도, gray scale은 8비트
    grayimg = (ubyte*)malloc(sizeof(ubyte)*graySize);                                                // gray scale을 위한 메모리 할당
    for(y = 0; y < bmpInfoHeader.biHeight; y++) {                               
        for(x = 0; x < size; x+=elemSize) {
            ubyte b = inimg[x+y*size+0];
            ubyte g = inimg[x+y*size+1];
            ubyte r = inimg[x+y*size+2];
            grayimg[x/elemSize+y*imageWidth]=((66*r+129*g+25*b+128)>>8)+16;                          // rgb를 이용해서 y를 구하는 공식으로 gray scale 구함
        }
    }

    /* make padding image */
    int padSize = (imageWidth + 2);                                                                  // 패딩 이미지의 width
    int addSize = (padSize + bmpInfoHeader.biHeight)*2;                                              // 사이드 부분의 크기
    padimg = (ubyte*)malloc(sizeof(ubyte)*(graySize + addSize));                                     // 패딩 이미지의 메모리 할당
    memset(padimg, 0, (sizeof(ubyte)*graySize + addSize));                                           // 패딩 0으로 초기화

    for(y = 0; y < bmpInfoHeader.biHeight; y++) {                                                    // 사이드는 0으로, 사이드가 아닌 부분은 gray scale이미지로 설정
        for(x = 0; x < imageWidth; x++) {
            padimg[(x+1)+(y+1)*padSize]=grayimg[x+y*imageWidth];
        }
    }

    for(x = 0; x < bmpInfoHeader.biHeight; x++) {                                                    // 패딩 이미지의 사이드 부분 정의
        padimg[(x+1)*padSize]=grayimg[x*imageWidth];
        padimg[padSize-1+(x+1)*padSize]=grayimg[imageWidth-1+x*imageWidth];
    }

    for(x = 0; x < imageWidth+2; x++) { 
        padimg[x]=grayimg[x];
        padimg[x+(bmpInfoHeader.biHeight+1)*padSize]=grayimg[x+(bmpInfoHeader.biHeight-1)*imageWidth];
    }

//  padimg[x]=grayimg[x];
    padimg[padSize-1]=grayimg[imageWidth-1];                                                        // 패딩 이미지에서 4개의 끈 인덱스 값 할당
    padimg[(bmpInfoHeader.biHeight+1)*padSize]=grayimg[(bmpInfoHeader.biHeight-1)*imageWidth];
    padimg[(bmpInfoHeader.biHeight+1)*padSize+padSize-1]=grayimg[(bmpInfoHeader.biHeight-1)*imageWidth+imageWidth-1];

    // define the kernel
    float xKernel[3][3] = { {-1,  0,  1},                                                           // x필터
                            {-2,  0,  2},
                            {-1,  0,  1} };

    float yKernel[3][3] = { { 1,  2,  1},                                                           // y필터
                            { 0,  0,  0},
                            {-1, -2, -1} };

    outimg = (ubyte*)malloc(sizeof(ubyte)*graySize);                                                // 출력 이미지 메모리 할당
    memset(outimg, 0, sizeof(ubyte)*graySize);                                                      // 출력 이미지 0으로 초기화
    for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) { 
        for(x = 1; x < padSize; x++) {                                                              // 8비트라서 1씩 더함
            float xVal = 0.0, yVal = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    xVal += xKernel[i+1][j+1]*padimg[(x+i)+(y+j)*padSize];                          // x필터가 적용된 값 저장
                    yVal += yKernel[i+1][j+1]*padimg[(x+i)+(y+j)*padSize];                          // y필터가 적용된 값 저장
                }
            }
            outimg[(x-1)+(y-1)*imageWidth] = LIMIT_UBYTE(sqrt(xVal*xVal+yVal*yVal));                // sobel gray
        }
    }         

    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);                                                 // 팔레트, 255가지 색상 사용
    for(x = 0; x < 256; x++) {
        palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
        palrgb[x].rgbReserved = 0;
    }

    bmpInfoHeader.biBitCount = 8;                                                                   // 8비트 이미지로 변환
    bmpInfoHeader.SizeImage = imageWidth*bmpInfoHeader.biHeight;                                    // 이미지 사이즈 재설정
    bmpInfoHeader.biCompression = 0;                                
    bmpInfoHeader.biClrUsed = 0;                                                                    // 인덱스 색 0으로 설정
    bmpInfoHeader.biClrImportant = 0;
    
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
    bmpHeader.bfSize = bmpHeader.bfOffBits + bmpInfoHeader.SizeImage;
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) {                                                           // 쓰기용 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                            // 파일헤더 정보 파일에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                        // 인포헤더 정보 파일에 저장

    fwrite(palrgb, sizeof(RGBQUAD), 256, fp);                                                       // 팔레트 정보 파일에 저장

    fwrite(outimg, sizeof(ubyte), graySize, fp);                                                    // 출력 이미지 파일에 저장

    fclose(fp); 

    free(inimg); 
    free(grayimg);
    free(padimg);
    free(outimg);
    
    return 0;
}
