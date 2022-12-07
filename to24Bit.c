#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4) // padding

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)                                                     // 대소비교 함수
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) {                                                                   
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    RGBQUAD *palrgb;    // not array, pointer

   FILE* fp; 
   char input[128], output[128];
   
   int i, j, size; 
   float srcX, srcY;
   int index; 
   float r,g,b,gray;
   int graysize; 
   int index2;
   
   unsigned char *grayimg, *inimg, *outimg, *rgbquad;
   
   /* usage a.out in.bmp out.bmp */
   strcpy(input, argv[1]); 
   strcpy(output, argv[2]);
   
   
   if((fp=fopen(input, "rb")) == NULL) {                                                              // 파일 오픈
      fprintf(stderr, "Error : Failed to open file...₩n"); 
      return -1;
   }
   
    fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                                              // 파일에서 파일헤더 읽어서 구조체에 저장
    fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                                              // 파일에서 인포헤더 읽어서 구조체에 저장

    if(bmpInfoHeader.biBitCount == 8 && bmpInfoHeader.biClrUsed == 0) {                               // 이미지가 8비트이고 사용하는 색이 0개일 경우 256가지 색 사용
            bmpInfoHeader.biClrUsed = 256;
    }
   
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n",bmpInfoHeader.biBitCount);

    size=widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);                                 // 열의 개수
    graysize = widthbytes(8 * bmpInfoHeader.biWidth);                                                  // gray scale의 행열의 개수
   
    unsigned int imagesize;
    if(!imagesize) 
       imagesize=bmpInfoHeader.biHeight*size;                                                          // 원본 이미지의 행열의 개수
   
    printf("Image Size : %d\n",imagesize);
    printf("Used Color : %d\n",bmpInfoHeader.biClrUsed);

    inimg=(BYTE*)malloc(sizeof(BYTE)*imagesize);                                                      // 원본 이미지 위한 메모리 할당
    outimg=(BYTE*)malloc(sizeof(BYTE)*(bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*3));              // 출력 이미지 위한 메모리 할당
    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*bmpInfoHeader.biClrUsed);                               // 팔레트 메모리 할당

    fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp);                                      // 파일에서 원본 이미지 읽어서 inimg에 저장

    fread(inimg, sizeof(BYTE), imagesize, fp);                                                        // 원본 이미지 읽음

    for(int i=0; i<bmpInfoHeader.biClrUsed;i++) {                                                     
            printf("Index %d : %u %u %u %u\n",i, palrgb[i].rgbRed, palrgb[i].rgbGreen, palrgb[i].rgbBlue, palrgb[i].rgbReserved);
    }

    fclose(fp);                                                                                       // 파일 닫음  
   
    int mask = 0;

    for(int x=0; x < bmpInfoHeader.biBitCount; x++)                                                   // 마스크 설정, 1비트일 경우 0b00000001
        mask |= 0b1 << x;                                                                             // 4비트일 경우 0b00001111
    printf("%d\n",mask);                                                                              // 8비트일 경우 0b11111111

    float elemsize = bmpInfoHeader.biBitCount / 8.;                                                   // rgb 개수
    int pos = 0;                                                                                      

    for(i = 0; i < bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * elemsize; i++) {                  // 비트에 따른 24비트 이미지 변환
        int num = inimg[i];
        for (j = 8-bmpInfoHeader.biBitCount; j >= 0; j -= bmpInfoHeader.biBitCount)
        {
                int res = num >> j & mask;                                                            // 쉬프트 연산을 통해 인덱스 값 가져옴
                outimg[pos++] = palrgb[res].rgbBlue;                                                  // 해당 인덱스에 해당하는 색의 rgb 정보 가져옴
                outimg[pos++] = palrgb[res].rgbGreen;
                outimg[pos++] = palrgb[res].rgbRed;
        }
    }

    bmpInfoHeader.biBitCount = 24;                                                                    // 24비트 이미지로 변환
    bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;                     // 이미지 사이즈 재설정
    bmpInfoHeader.biClrUsed = 0;                                                                      // 팔레트 사용안하므로 0
    bmpFileHeader.bf0ffBits = sizeof(bmpInfoHeader) + sizeof(bmpFileHeader);                            
   
   if((fp=fopen(output, "wb"))==NULL) {                                                               // 쓰기용 파일 오픈
      fprintf(stderr, "Error : Failed to open file...₩n"); 
      return -1;
   }
   
   fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                                              // 파일헤더 파일에 저장
   fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                                              // 인포헤더 파일에 저장

   fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);                                // 출력 이미지 파일에 저장
   
   free(inimg); 
   free(outimg);
   
   fclose(fp); 
   
   return 0;
}