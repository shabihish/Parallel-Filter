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

vector<vector<Pixel>> applySmoothingFilter(vector<vector<Pixel>> image) {
    vector<vector<Pixel>> new_image = image;
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[i].size(); j++) {
            if (i == 0 || i == image.size() - 1 || j == 0 || j == image[i].size() - 1)
                new_image[i][j].setToZero();
            else
                new_image[i][j] =
                        (image[i - 1][j - 1] + image[i - 1][j] + image[i - 1][j + 1] + image[i][j - 1] + image[i][j] +
                         image[i][j + 1] + image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1]) / 9.0;
        }
    }
    return new_image;
}

vector<vector<Pixel>> applySepiaFilter(vector<vector<Pixel>> image) {
    vector<vector<Pixel>> new_image = image;
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[i].size(); j++) {
            new_image[i][j].applySepia();
        }
    }
    return new_image;
}