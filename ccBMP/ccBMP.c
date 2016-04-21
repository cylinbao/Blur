#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef long UINT32;
typedef unsigned short int UINT16;
typedef unsigned char BYTE;

#define HEADER_SIZE 14
#define INFO_SIZE 40
#define DataSize(bmp) (bmp->width*bmp->height*3)

typedef struct BMP {
    BYTE header[HEADER_SIZE];    
    BYTE info[INFO_SIZE];
    // Header
    UINT16 signature; // Magic Number = "BM" = 0x4D42
    UINT32 fileSize; // File size in bytes
    UINT32 hreserved; // unused (=0)
    UINT32 dataOffset; // File offset to Raster Data
    // InfoHeader
    UINT32 size; // Size of InfoHeader =40 
    UINT32 width; // Bitmap Width
    UINT16 height; // Bitmap Height
    UINT16 planes; // Number of Planes (=1)
    UINT16 bitsPerPixel; // Bits per Pixel, 1, 4, 8, 16, 24
    UINT32 compression; // Type of Compression, 0 = BI_RGB no compression, 1 = BI_RLE8 8bit RLE encoding, 2 = BI_RLE4 4bit RLE encoding
    UINT32 imageSize; // (compressed) Size of Image, It is valid to set this =0 if Compression = 0
    UINT32 xPixelsPerM; // horizontal resolution: Pixels/meter
    UINT32 yPixelsPerM; // vertical resolution: Pixels/meter
    UINT32 colorsUsed; // Number of actually used colors
    UINT32 colorsImportant; // Number of important colors , 0 = all
    // ColorTable : present only if Info.BitsPerPixel <= 8 colors should be ordered by importance
    BYTE blue; // Blue intensity
    BYTE green; // Green intensity
    BYTE red; // Red intensity
    BYTE creserved; // unused (=0)
    // Raster Data
    BYTE *data;
} BMP;

typedef struct Pixel {
    BYTE R;
    BYTE G;
    BYTE B;
} Pixel;

#define U16(x)    ((unsigned short) (x))
#define U32(x)    ((int) (x))
#define B2U16(bytes,offset)  (U16(bytes[offset]) | U16(bytes[offset+1]) << 8)
#define B2U32(bytes,offset)  (U32(bytes[offset]) | U32(bytes[offset+1]) << 8 | \
                   U32(bytes[offset+2]) << 16 | U32(bytes[offset+3]) << 24)

int bmpLoad(BMP *bmp, char *fileName) {
   FILE *file;
   BYTE header[14];
   BYTE info[40];

   if ((file = fopen(fileName, "rb") ) == NULL ) {
      fprintf(stderr, "Error: bmpLoad(), File open fail!\n");
      exit(0);
   }

   fread(header, 1, HEADER_SIZE, file);
   fread(info, 1, INFO_SIZE, file);
   memcpy(bmp->header, header, HEADER_SIZE);
   memcpy(bmp->info, info, INFO_SIZE);
   // Header
   bmp->signature = B2U16(header,0); assert(bmp->signature == 0x4D42);
   bmp->fileSize = B2U32(header,2);
   bmp->dataOffset = B2U32(header,10);
   // InfoHeader
   bmp->size = B2U32(info,0); assert(bmp->size==40);
   bmp->width = B2U32(info,4);
   bmp->height = B2U32(info,8);
   bmp->planes = B2U16(info,12); assert(bmp->planes==1);
   bmp->bitsPerPixel = B2U16(info,14); assert(bmp->bitsPerPixel==24);
   bmp->compression = B2U32(info,16);
   bmp->imageSize = B2U32(info,20);
   bmp->xPixelsPerM = B2U32(info,24);
   bmp->yPixelsPerM = B2U32(info,28);
   bmp->colorsUsed = B2U32(info,32);
   bmp->colorsImportant = B2U32(info,36);

   bmp->data = malloc(DataSize(bmp));
   fseek(file, bmp->dataOffset, SEEK_SET);
   fread(bmp->data, 1, DataSize(bmp), file);

   fclose (file);
}

int bmpPrint(BMP *bmp) {
   printf("==== Header ====\n");
   printf("Signature = %04X\n", bmp->signature); // 0x4d42 = BM
   printf("FileSize = %ld \n", bmp->fileSize);
   printf("DataOffset = %ld \n", bmp->dataOffset);
   printf("==== Info ======\n");
   printf("size = %ld \n", bmp->size);
   printf("Width = %ld \n", bmp->width);
   printf("Height = %ld \n", bmp->height);
   printf("Planes = %d \n", bmp->planes);
   printf("BitsPerPixel = %d \n", bmp->bitsPerPixel);
   printf("Compression = %ld \n", bmp->compression);
   printf("ImageSize = %ld \n", bmp->imageSize);
   printf("XpixelsPerM = %ld \n", bmp->xPixelsPerM);
   printf("YpixelsPerM = %ld \n", bmp->yPixelsPerM);
   printf("ColorsUsed = %ld \n", bmp->colorsUsed);
   printf("ColorsImportant = %ld \n", bmp->colorsImportant);
}

int bmpSetPixel(BMP *bmp, int x, int y, BYTE R, BYTE G, BYTE B) {
    Pixel *pixel;
    int idx = y * bmp->width + x;
    pixel = (Pixel*) &bmp->data[idx * sizeof(Pixel)];
    pixel->R = R;
    pixel->G = G;
    pixel->B = B;
}

int bmpSetBox(BMP *bmp, int x, int y, int w, int h, BYTE R, BYTE G, BYTE B) {
    int i,j;
    for (i=0; i< 0+w; i++)
        for (j=0; j<h; j++)
            bmpSetPixel(bmp, x+i, y+j, R, G, B);
}

int bmpSave(BMP *bmp, char *fileName) {
    FILE *file;
    if ((file = fopen(fileName, "wb") ) == NULL ) {
        fprintf(stderr, "Error: bmpSave(), File create fail!\n");
        exit(0);
    }
    fwrite(bmp->header, 1, HEADER_SIZE, file);
    fwrite(bmp->info, 1, INFO_SIZE, file);
    fseek(file, bmp->dataOffset, SEEK_SET);
    fwrite(bmp->data, 1, DataSize(bmp), file);
    fclose(file);
}

int main(int argc, char *argv[]) {
    BMP bmp;
    bmpLoad(&bmp, argv[1]);
    bmpPrint(&bmp);
    //bmpSetBox(&bmp, 0, 0, 20, 20, 0x66, 0x33, 0x99);
    bmpSave(&bmp, argv[2]);
    return 0;
}
