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
    BITMAPFILEHEADER bmpHeader;                                         
    BITMAPINFOHEADER bmpInfoHeader;    
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg;
    int x, y, z, imageSize;

    if(argc != 3) {                                                                     // 인자가 3개가 아니면 에러
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) {                                             // 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                 // 파일에서 파일헤더 읽어서 구조체에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                             // 파일에서 인포헤더 읽어서 구조체에 저장

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {                                                // 24비트 지원안하면 실행 안함
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;                                          // 요소의 개수, 24 / 8 = 3 
    int size = bmpInfoHeader.biWidth*elemSize;                                          // 열의 개수
    imageSize = size * bmpInfoHeader.biHeight;                                          // 전체 행열의 개수

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);                            // 해상도 출력
    printf("Bit Count : %d(%d)\n", bmpInfoHeader.biBitCount, elemSize);                                         // 픽셀당 비트수 출력
    printf("Image Size : %d\n", imageSize);                                                                     // 전체 행열의 개수 출력

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                                            // 원본파일 받을 메모리 할당
    outimg = (ubyte*)malloc(sizeof(ubyte)*bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);                        // 출력할 파일 받을 메모리 할당
    fread(inimg, sizeof(ubyte), imageSize, fp);                                                                 // 파일에서 원본파일 읽어서 inimg에 저장
    
    fclose(fp);
    
    for(y = 0; y < bmpInfoHeader.biHeight; y++) {
        for(x = 0; x < size; x+=elemSize) {
            ubyte b = inimg[x+y*size+0];                                                                        // red
            ubyte g = inimg[x+y*size+1];                                                                        // green
            ubyte r = inimg[x+y*size+2];                                                                        // blue
            outimg[x/elemSize+y*bmpInfoHeader.biWidth]=(r*0.299F)+(g*0.587F)+(b*0.114F);                        // 24비트를 8비트로 압축해서 저장
        }
    }
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) {                                                                       // 쓰기용 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);                                                             // 팔레트 할당
    for(x = 0; x < 256; x++) {
        palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;                                          // 팔레트 색상 할당
        palrgb[x].rgbReserved = 0;                                                                              // 투명도 0
    }

    bmpInfoHeader.biBitCount = 8;                                                                               // 픽셀당 비트 수, 1바이트
	bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*bmpInfoHeader.biBitCount/8;          // 바이트 단위로 사이즈를 표시하기 위해 8로 나눔
    bmpInfoHeader.biCompression = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;

    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
    bmpHeader.bfSize = bmpHeader.bfOffBits + bmpInfoHeader.SizeImage;

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                                        // 파일에 파일헤더 정보 씀

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                                    // 파일에 인포헤더 정보 씀

    fwrite(palrgb, sizeof(RGBQUAD), 256, fp);                                                                   // 파일에 팔레트 정보 씀

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), bmpInfoHeader.biWidth*bmpInfoHeader.biHeight, fp);                            // 변경된 이미지 파일에 저장

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}
