#include <iostream>
#include <unistd.h>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct {
    short r;
    short g;
    short b;
} pixel;

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize) {
    std::ifstream file(fileName);

    if (file) {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        bufferSize = file_header->bfSize;
        return 1;
    } else {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return 0;
    }
}

pixel *getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer) {
    int c = 0;
    int count = 1;
    int extra = cols % 4;

    auto *image = static_cast<pixel *>(calloc(sizeof(pixel), rows * cols));
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            pixel p;
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        // fileReadBuffer[end - count] is the red value
                        p.r = fileReadBuffer[end - count];
                        break;
                    case 1:
                        // fileReadBuffer[end - count] is the green value
                        p.g = fileReadBuffer[end - count];
                        break;
                    case 2:
                        // fileReadBuffer[end - count] is the blue value
                        p.b = fileReadBuffer[end - count];
                        break;
                        // go to the next position in the buffer
                }
            }
            image[c] = p;
            c++;
        }
    }
    return image;
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, pixel* image) {
    std::ofstream write(nameOfFileToCreate);
    if (!write) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }

    int c = 0;
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        // write red value in fileBuffer[bufferSize - count]
                        fileBuffer[bufferSize - count] = image[c].r;
                        break;
                    case 1:
                        // write green value in fileBuffer[bufferSize - count]
                        fileBuffer[bufferSize - count] = image[c].g;
                        break;
                    case 2:
                        // write blue value in fileBuffer[bufferSize - count]
                        fileBuffer[bufferSize - count] = image[c].b;
                        break;
                        // go to the next position in the buffer
                }
            }
            c++;
        }
    }
    write.write(fileBuffer, bufferSize);
}

int main(int argc, char *argv[]) {
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize)) {
        cout << "File read error" << endl;
        return 1;
    }

    // read input file
    pixel* image = getPixlesFromBMP24(bufferSize-1, rows, cols, fileBuffer);

    // apply filters
    // write output file
    writeOutBmp24(fileBuffer, "new.bmp", bufferSize, image);

    return 0;
}