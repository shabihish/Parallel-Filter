#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include "filters.hpp"
#include <pthread.h>
#include <unistd.h>

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

class ThreadingArgs {
public:
    int start;
    int chunkSize;
    int bufferSize;
    int rows;
    int cols;
    char *buffer;
    vector<vector<Pixel>> *image;

    ThreadingArgs(int start = 0, int chunkSize = 0, int bufferSize = 0, int rows = 0, int cols = 0,
                  char *buffer = nullptr, vector<vector<Pixel>> *image = nullptr) {
        this->start = start;
        this->chunkSize = chunkSize;
        this->bufferSize = bufferSize;
        this->rows = rows;
        this->cols = cols;
        this->buffer = buffer;
        this->image = image;
    }
};

void *getPixlesFromBMP24(ThreadingArgs *args) {
    int count = args->start;
    int extra = args->cols % 4;

//    *args->image = vector<vector<Pixel>>(args->rows);
    for (int i = 0; i < args->rows; i++) {
        args->image->emplace_back(vector<Pixel>());
        count += extra;
        for (int j = 0; j < args->cols; j++) {
            (*args->image)[i].emplace_back(Pixel());
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        // buffer[bufferSize - count] is the red value
                        (*args->image)[i][j].b = (unsigned char) args->buffer[count];
                        break;
                    case 1:
                        // buffer[bufferSize - count] is the green value
                        (*args->image)[i][j].g = (unsigned char) args->buffer[count];
                        break;
                    case 2:
                        // buffer[bufferSize - count] is the blue value
                        (*args->image)[i][j].r = (unsigned char) args->buffer[count];
                        break;
                        // go to the next position in the buffer
                }
                count++;
//                if (count - args->start == args->chunkSize)
//                    return nullptr;
            }
        }
    }

    return nullptr;
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
                        // write red value in buffer[bufferSize - count]
                        fileBuffer[count] = (char) (image[i][j].b);
                        break;
                    case 1:
                        // write green value in buffer[bufferSize - count]
                        fileBuffer[count] = (char) (image[i][j].g);
                        break;
                    case 2:
                        // write blue value in buffer[bufferSize - count]
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


void *threadsFunc(ThreadingArgs *args) {
    // read input file
//    pthread_t thread;
//    pthread_create(nullptr, nullptr, nullptr, nullptr);
//    pthread_create(&thread, &attr, reinterpret_cast<void *(*)(void *)>(getPixlesFromBMP24), &args);
    getPixlesFromBMP24(args);

    *args->image = applySmoothingFilter(*args->image, args->rows, args->cols);
    *args->image = applySepiaFilter(*args->image, args->rows, args->cols);
    return nullptr;
}


pthread_t runThread(ThreadingArgs &args, pthread_attr_t &attr) {
    // read input file
    pthread_t thread;
//    pthread_create(nullptr, nullptr, nullptr, nullptr);
    pthread_create(&thread, &attr, reinterpret_cast<void *(*)(void *)>(getPixlesFromBMP24), &args);
//    vector<vector<Pixel>> image = getPixlesFromBMP24(args.start, rows, cols, args.buffer);

    void *status;
    pthread_join(thread, &status);

//    args.image = applySmoothingFilter(args.image, args.rows, args.cols);
//    args.image = applySepiaFilter(args.image, args.rows, args.cols);
//    args.image = applyOverallMeanFilter(args.image);
//    args.image = addCrossToImage(args.image);
    return thread;
}

int main(int argc, char *argv[]) {
    char *fileBuffer;
    int bufferSize, headerSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize, headerSize)) {
        cout << "File read error" << endl;
        return 1;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int numOfThreads = 5;
    pthread_t threads[numOfThreads];

    int basicChunkSizes = (bufferSize - headerSize) / numOfThreads;
    int basicRowSizes = basicChunkSizes / (rows * 3);
    if (basicChunkSizes % (rows * 3) != 0) {
        cout << "Invalid number of threads entered." << endl;
        exit(-1);
    }

    vector<vector<Pixel>> images[numOfThreads];

    int rc = 0;
    ThreadingArgs args[numOfThreads];

    long s = getTime();
    int chunkSize, start, rowSize;
    for (int i = 0; i < numOfThreads; i++) {
        start = headerSize + basicChunkSizes * i;
        rowSize = basicRowSizes;
        if (i > 0) {
            start -= cols * 3;
            rowSize += 1;
        }

        if (i < numOfThreads - 1)
            rowSize += 1;

        args[i] = ThreadingArgs(start, basicChunkSizes, bufferSize, rowSize, cols, fileBuffer,
                                &images[i]);
        rc = pthread_create(&threads[i], &attr, reinterpret_cast<void *(*)(void *)>(threadsFunc), &args[i]);
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    vector<vector<Pixel>> image;

    void *status;
    pthread_attr_destroy(&attr);
    for (int i = 0; i < numOfThreads; i++) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }

        auto beginning = images[i].begin();
        if (i > 0)
            beginning += 1;
        auto end = images[i].end();
        if (i < numOfThreads - 1)
            end -= 1;
        image.insert(image.end(), beginning, end);
    }

    image = applyOverallMeanFilter(image);
    image = addCrossToImage(image);

    // write output file
    writeOutBmp24(fileBuffer, "new2.bmp", bufferSize, headerSize, image);

    cout << getTime() - s << "us" << endl;
    return 0;
}
