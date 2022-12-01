#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include "bmpHeader.h"

#define widthBytes(bits) (((bits)+31)/32*4)

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv)
{
    FILE* fp; 										// 파일 
    BITMAPFILEHEADER bmpHeader;            							// 파일 헤더
    BITMAPINFOHEADER bmpInfoHeader;     							// 인포 헤더
    RGBQUAD* palrgb;									// 팔레트
    ubyte* inimg, * outimg;									// 입력으로 받을 인덱스 이미지와 출력으로 보낼 24비트 이미지를 위한 메모리
    int x, y, z, elemSize, imageSize;

    if (argc != 3) {										// 인자가 3개가 아니면 에러
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }

    /***** read bmp *****/
    if ((fp = fopen(argv[1], "rb")) == NULL) { 							// 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n");
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);						// 파일로 부터 파일헤더 읽어서 구조체에 저장 

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);					// 파일로 부터 인포헤더를 읽어서 구조체에 저장

    if (imageSize != 0)									// 인덱스 이미지의 메모리 크기 정의
        imageSize = widthBytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth) *
        bmpInfoHeader.biHeight;

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);		// 해상도 출력
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */			// 픽셀당 비트수 출력
    printf("Image Size : %d\n", imageSize);							// 이미지 크기 출력
    printf("Color : %d\n", bmpInfoHeader.biClrUsed);						// 사용된 색상 수 출력

    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * bmpInfoHeader.biClrUsed);				// 팔레트 메모리 할당
    fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp); 					// 파일에서 사용된 인덱스 칼라 정보를 읽어서 팔레트에 저장

    for (int i = 0; i < bmpInfoHeader.biClrUsed; i++) 						// 인덱스와 인덱스에 해당하는 칼라의 rgb 출력
        printf("%d : %x %x %x %x\n", i, palrgb[i].rgbRed, palrgb[i].rgbGreen,
            palrgb[i].rgbRed, palrgb[i].rgbReserved);
    //printf("%d %d\n", sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

    inimg = (ubyte*)malloc(sizeof(ubyte) * imageSize);						// 인덱스 이미지 메모리 할당 
    outimg = (ubyte*)malloc(sizeof(ubyte) * (bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3));		// 24비트 이미지 메모리 할당, 해상도 * 3바이트
    fread(inimg, sizeof(ubyte), imageSize, fp); 							// 파일에서 인덱스 이미지 읽음

    fclose(fp);										// 파일 닫기

    elemSize = 3; 										// rgb
    int pos = 0;

    for (x = 0; x < bmpInfoHeader.biWidth * bmpInfoHeader.biHeight / 8; x++) { 				// 인덱스 이미지의 크기를 바이트 단위로 나타낸 것이며 x가 1증가할 때 1바이트 증가
        for (int i = 7; i >= 0; --i) {								// 마스킹을 위해 7비트 만큼 데이터를 밈
            int num = inimg[x]; 								// unsinged char는 8비트 이므로 한번에 1바이트만큼 인덱스 값 가져옴
            int res = num >> i & 1;								// 마스킹해서 비트당 인덱스 값을 저장
            outimg[pos++] = palrgb[res].rgbBlue;							// 인덱스에 맞는 rgb 출력
            outimg[pos++] = palrgb[res].rgbGreen;
            outimg[pos++] = palrgb[res].rgbRed;
        }
    }

    /***** write bmp *****/
    if ((fp = fopen(argv[2], "wb")) == NULL) { 							// 쓰기용 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n");
        return -1;
    }

    bmpInfoHeader.biBitCount = 24;										// 픽셀당 비트 수 설정
    bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;				// 24비트 파일 크기 설정
    bmpInfoHeader.biClrUsed = 0;										// 사용된 색상 수 설정
    bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfoHeader.SizeImage;
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);								// 파일헤더의 정보를 파일에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);							// 인포헤더의 정보를 파일에 저장

    fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);						// 24비트 이미지 파일에 저장

    fclose(fp); 												// 파일 닫기

    free(inimg);
    free(outimg);

    return 0;
}