#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <time.h>

#include "bmpHeader.h"

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)                                             // clip 함수, 값이 255이상이면 255로, 0보다 작으면 0으로 설정
{
    return(value > max ? max : value < min ? min : value);
}

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    ubyte *inimg;
    int i, z, cnt, elemSize, size, imageSize;
    srand((unsigned int)time(NULL)); 	                                                            // seed값으로 현재시간 부여

    if(argc != 4) {                                                                                 // 인자가 4개가 아니면 에러
        fprintf(stderr, "usage : %s count input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[2], "rb")) == NULL) {                                                         // 입력 파일 오픈
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                             // 파일에서 파일헤더 읽어서 구조체에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                         // 파일에서 인포헤더 읽어서 구조체에 저장

    /* 트루 컬러를 지원하지 않으면 표시할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {                                                            // 24비트 지원안하면 실행 안함
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    elemSize = bmpInfoHeader.biBitCount / 8;                                                        // rgb 개수
    size = bmpInfoHeader.biWidth * elemSize;                                                        // 열의 개수
    imageSize = bmpInfoHeader.biWidth * elemSize * bmpInfoHeader.biHeight;                          // 행열의 개수

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);
    
    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);                                                // 원본 이미지를 위한 메모리 할당
    outimg = (ubyte*)malloc(sizeof(ubyte) * imageSize);                                             // 출력 이미지를 위한 메모리 할당
    fread(inimg, sizeof(ubyte), imageSize, fp);                                                     // 파일에서 원본 이미지 읽어서 inimg에 저장
    
    fclose(fp);                                                                                     // 파일 닫기
    
                                                                                                                                        
    for(i = 0; i < *argv[1]; i++) {                                                                 // 노이즈 개수만큼 반복
        int x = rand() & bmpInfoHeader.biWidth * elemSize;                                          // 이미지 범위 내의 랜덤한 x좌표
        int y = rand() & bmpInfoHeader.biHeight;                                                    // 이미지 범위 내의 랜덤한 y좌표
        int value = rand( ) & 0b11111111;                                                           // 0부터 255 사이의 난수
        for(z = 0; z < elemSize; z++) {                                                                          
            int tmp = inimg[x + y * size + z] + value;                                              // 원본 이미지 값에 노이즈 추가
            outimg[x + y * size + z] = clip(tmp, 0, 255);                                            // 노이즈가 추가된 값이 255를 넘으면 255로 고정
        }
     }         
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[3], "wb"))==NULL) {                                                           // 쓰기용 파일 생성
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }
    
    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);                                            // 파일헤더 읽어서 파일에 저장

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);                                        // 인포헤더 읽어서 파일에 저장

    fwrite(outimg, sizeof(unsigned char), imageSize, fp);                                           // 출력 파일 읽어서 파일에 저장 
    
    free(inimg);
    free(outimg);
    
    fclose(fp); 
    
    return 0;
}
