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

Pixel &Pixel::operator=(const Pixel &pixel) = default;

Pixel &Pixel::operator+(const Pixel &pixel) {
    this->r += pixel.r;
    this->g += pixel.g;
    this->b += pixel.b;

    return *this;
};

Pixel &Pixel::operator/(const int dividend) {
    this->r /= dividend;
    this->g /= dividend;
    this->b /= dividend;

    return *this;
};

vector<vector<Pixel>> applySmoothing(vector<vector<Pixel>> image) {
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[i].size(); j++) {
//            if (i == 0 || i == image.size() - 1 || j == 0 || j == image[i].size() - 1)
            image[i][j].setToZero();

//            image[i][j] = (image[i - 1][j - 1] + image[i - 1][j] + image[i - 1][j + 1] + image[i][j - 1] + image[i][j] +
//                           image[i][j + 1] + image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1]) / 9.0;
        }
    }
    return image;
}