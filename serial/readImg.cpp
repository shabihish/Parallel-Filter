#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/time.h>
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

//struct timespec {
//    time_t   tv_sec;        /* seconds */
//    long     tv_nsec;       /* nanoseconds */
//} ts;

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
                        image[i][j].b = (unsigned char) fileReadBuffer[count];
                        break;
                    case 1:
                        // fileReadBuffer[bufferSize - count] is the green value
                        image[i][j].g = (unsigned char) fileReadBuffer[count];
                        break;
                    case 2:
                        // fileReadBuffer[bufferSize - count] is the blue value
                        image[i][j].r = (unsigned char) fileReadBuffer[count];
                        break;
                        // go to the next position in the buffer
                }
                count++;
            }
        }
    }

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
                        fileBuffer[count] = (char) (image[i][j].b);
                        break;
                    case 1:
                        // write green value in fileBuffer[bufferSize - count]
                        fileBuffer[count] = (char) (image[i][j].g);
                        break;
                    case 2:
                        // write blue value in fileBuffer[bufferSize - count]
                        fileBuffer[count] = (char) (image[i][j].r);
                        break;
                        // go to the next position in the buffer
                }
                count++;
            }
        }
    }
    write.write(fileBuffer, bufferSize);
}

struct timeval tv;

uint64_t getTime() {
    gettimeofday(&tv, nullptr);
    return tv.tv_usec + tv.tv_sec * (uint64_t) 1000000;
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
    uint64_t s1 = getTime();
    vector<vector<Pixel>> image = getPixlesFromBMP24(headerSize, rows, cols, fileBuffer);
    uint64_t t1 = getTime() - s1;

    // apply filters
    uint64_t s2 = getTime();
    image = applySmoothingFilter(image);
    uint64_t t2 = getTime() - s2;

    uint64_t s3 = getTime();
    image = applySepiaFilter(image);
    uint64_t t3 = getTime() - s3;

    uint64_t s4 = getTime();
    image = applyOverallMeanFilter(image);
    uint64_t t4 = getTime() - s4;

    uint64_t s5 = getTime();
    image = addCrossToImage(image);
    uint64_t t5 = getTime() - s5;

    // write output file
    uint64_t s6 = getTime();
    writeOutBmp24(fileBuffer, "new1.bmp", bufferSize, headerSize, image);
    uint64_t t6 = getTime() - s6;

    double total = t1 + t2 + t3 + t4 + t5 + t6;
    cout << "t1: " << t1 / total << endl;
    cout << "t2: " << t2 / total << endl;
    cout << "t3: " << t3 / total << endl;
    cout << "t4: " << t4 / total << endl;
    cout << "t5: " << t5 / total << endl;
    cout << "t6: " << t6 / total << endl;

    cout << "\ntotal: " << total << "us" << endl;

    return 0;
}