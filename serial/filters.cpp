#include <cstdlib>
#include "filters.hpp"

using namespace std;

Pixel::Pixel() {
    this->setToZero();
}

void Pixel::setToZero() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

void Pixel::applySepia() {
    int new_r = (int) ((this->r * 0.393) + (this->g * 0.769) + (this->b * 0.189));
    int new_g = (int) ((this->r * 0.349) + (this->g * 0.686) + (this->b * 0.168));
    int new_b = (int) ((this->r * 0.272) + (this->g * 0.534) + (this->b * 0.131));

    this->r = new_r > 255 ? 255 : new_r;
    this->g = new_g > 255 ? 255 : new_g;
    this->b = new_b > 255 ? 255 : new_b;
}

Pixel &Pixel::operator=(const Pixel &pixel) {
    this->r = pixel.r;
    this->g = pixel.g;
    this->b = pixel.b;

    return *this;
}

Pixel &Pixel::operator=(const int b) {
    this->r = b;
    this->g = b;
    this->b = b;

    return *this;
}

Pixel Pixel::operator+(const Pixel &pixel) {
    Pixel p;
    p.r = this->r + pixel.r;
    p.g = this->g + pixel.g;
    p.b = this->b + pixel.b;

    return p;
};

Pixel Pixel::operator/(const double dividend) {
    Pixel p;
    p.r = (int) (this->r / dividend);
    p.g = (int) (this->g / dividend);
    p.b = (int) (this->b / dividend);

    return p;
};

Pixel Pixel::operator*(const double b) {
    Pixel p;
    p.r = (int) (this->r * b);
    p.g = (int) (this->g * b);
    p.b = (int) (this->b * b);

    return p;
};

vector<vector<Pixel>> applySmoothingFilter(vector<vector<Pixel>> image) {
    vector<vector<Pixel>> new_image = image;
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[i].size(); j++) {
            if (i == 0 || i == image.size() - 1 || j == 0 || j == image[i].size() - 1)
//                new_image[i][j].setToZero();
                continue;
            else
                new_image[i][j] =
                        (image[i - 1][j - 1] + image[i - 1][j] + image[i - 1][j + 1] + image[i][j - 1] + image[i][j] +
                         image[i][j + 1] + image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1]) / 9.0;
        }
    }
    return new_image;
}

vector<vector<Pixel>> applySepiaFilter(vector<vector<Pixel>> image) {
    for (auto &i: image) {
        for (auto &j: i) {
            j.applySepia();
        }
    }
    return image;
}

vector<vector<Pixel>> applyOverallMeanFilter(vector<vector<Pixel>> image) {
    Pixel p;
    p.setToZero();
    for (auto &i: image) {
        for (auto &j: i) {
            p = p + j;
        }
    }
    p = p / (double) (image.size() * image[0].size());

    for (auto &i: image) {
        for (auto &j: i) {
            j = j * 0.4 + p * 0.6;
        }
    }
    return image;
}

vector<vector<Pixel>> addCrossToImage(vector<vector<Pixel>> image) {
    int i = 0;
    int end = min(image.size(), image[0].size());
    int rows = image.size();
    int cols = image[0].size();

    for (i = 0; i < end; i++) {
        image[i][i] = 255;
        if (i + 1 < rows)
            image[i + 1][i] = 255;
        if (i + 1 < cols)
            image[i][i + 1] = 255;

        image[end - i - 1][i] = 255;
        if (end - i - 2 >= 0)
            image[end - i - 2][i] = 255;
        if (i + 1 < cols)
            image[end - i - 1][i + 1] = 255;
    }
    return image;
}