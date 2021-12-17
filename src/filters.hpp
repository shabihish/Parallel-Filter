#ifndef CA3_FILTERS_HPP
#define CA3_FILTERS_HPP

#include <vector>

using namespace std;

class Pixel {
public:
    int r;
    int g;
    int b;

    Pixel();

    void setToZero();

    Pixel &operator=(const Pixel &pixel);

    Pixel operator+(const Pixel &pixel);

    Pixel operator/(const float dividend);
};

vector<vector<Pixel>> applySmoothing(vector<vector<Pixel>> image, int rows, int cols);

#endif
