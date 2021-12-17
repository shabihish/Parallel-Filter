#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "filters.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;

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

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize, int &headerSize) {
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
        headerSize = bufferSize - info_header->biSizeImage;
        return 1;
    } else {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return 0;
    }
}

vector<vector<Pixel>> getPixlesFromBMP24(int headerSize, int rows, int cols, char *fileReadBuffer) {
    int count = headerSize;
    int extra = cols % 4;

//    Pixel **image = static_cast<Pixel **>(calloc(sizeof(Pixel *), rows));
    vector<vector<Pixel>> image(rows);
    for (int i = 0; i < rows; i++) {
        image.emplace_back(vector<Pixel>(cols));
//         rowPixels(cols);
        count += extra;
        for (int j = 0; j < cols; j++) {
            image[i].emplace_back(Pixel());
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        // fileReadBuffer[bufferSize - count] is the red value
                        image[i][j].r = (unsigned char)fileReadBuffer[count];
                        break;
                    case 1:
                        // fileReadBuffer[bufferSize - count] is the green value
                        image[i][j].g = (unsigned char)fileReadBuffer[count];
                        break;
                    case 2:
                        // fileReadBuffer[bufferSize - count] is the blue value
                        image[i][j].b = (unsigned char)fileReadBuffer[count];
                        break;
                        // go to the next position in the buffer
                }
                count++;
            }
        }
    }

    int max = -1000;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Pixel p = image[i][j];
            if (p.r > max)
                max = p.r;
            if (p.g > max)
                max = p.g;
            if (p.b > max)
                max = p.b;
        }
    }
    cout << "MAX: " << max << endl;
    return image;
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, int headerSize,
                   vector<vector<Pixel>> image) {
    std::ofstream write(nameOfFileToCreate);
    if (!write) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }

    int count = headerSize;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        // write red value in fileBuffer[bufferSize - count]
                        fileBuffer[count] = (char)image[i][j].r;
                        break;
                    case 1:
                        // write green value in fileBuffer[bufferSize - count]
                        fileBuffer[count] = (char)image[i][j].g;
                        break;
                    case 2:
                        // write blue value in fileBuffer[bufferSize - count]
                        fileBuffer[count] = (char)image[i][j].b;
                        break;
                        // go to the next position in the buffer
                }
                count++;
            }
        }
    }
    write.write(fileBuffer, bufferSize);
}

int main(int argc, char *argv[]) {
    char *fileBuffer;
    int bufferSize, headerSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize, headerSize)) {
        cout << "File read error" << endl;
        return 1;
    }

    // read input file
    vector<vector<Pixel>> image = getPixlesFromBMP24(headerSize, rows, cols, fileBuffer);

    // apply filters
    image = applySmoothing(image, rows, cols);
    // write output file

    writeOutBmp24(fileBuffer, "new.bmp", bufferSize, headerSize, image);

    return 0;
}