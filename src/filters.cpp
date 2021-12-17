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

Pixel Pixel::operator/(const float dividend) {
    Pixel p;
    p.r = this->r / dividend;
    p.g = this->g / dividend;
    p.b = this->b / dividend;

    return p;
};

vector<vector<Pixel>> applySmoothing(vector<vector<Pixel>> image) {
    vector<vector<Pixel>> new_image = image;
    new_image[0][0].r +=1;
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