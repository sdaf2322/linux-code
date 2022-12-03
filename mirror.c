#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg;
    int x, y, z, imageSize;

    if(argc != 4) {
        fprintf(stderr, "usage : %s -[h/v] input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[2], "rb")) == NULL) {                                                     // 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                         // 파일에서 파일 헤더 읽어서 구조체에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                     // 파일에서 인포 헤더 읽어서 구조체에 저장

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {                                                        // 24비트를 지원하지 않으면 실행 안함
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;                                                  // 픽셀의 요소의 개수, 24/8 = 3개
    int size = bmpInfoHeader.biWidth*elemSize;                                                  // width * 요소의 개수 = 열의 개수
    imageSize = size * bmpInfoHeader.biHeight;                                                  // 전체 행 열의 개수

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);            // 해상도 출력
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);                                       // 픽셀당 비트수 출력
    printf("Image Size : %d\n", imageSize);                                                     // 전체 행 열의 개수 출력

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                            // 입력 이미지 메모리 할당
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                           // 출력 이미지 메모리 할당
    fread(inimg, sizeof(ubyte), imageSize, fp);                                                 // 파일로부터 이미지 읽음
    
    fclose(fp);                                                                                 // 파일 닫기
    
    char ch = getopt(argc, argv, "hv:");                                                        // 
    for(y = 0; y < bmpInfoHeader.biHeight; y++) { 
        for(x = 0; x < size; x+=elemSize) {                                                     
            for(z = 0; z < elemSize; z++) {
                switch(ch) {
                  case 'h': 
                      outimg[x+y*size+z] = inimg[size-x-elemSize+y*size+z];                     // 수평 대칭, outimg[y][size-x]를 1차원 배열로 만든 것
                      break;
                  case 'v': default:
                      outimg[x+y*size+z] = inimg[x+(bmpInfoHeader.biHeight-y)*size+z];          // 수직 대칭, outimg[height-y][x]를 1차원 배열로 만든 것
                      break;
                }
            }
        }
    }         
     
    /***** write bmp *****/ 
    f((fp=fopen(argv[3], "wb"))==NULL) {                                                        // 쓰기용 파일 생성
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                        // 파일에 구조체 데이터 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}
