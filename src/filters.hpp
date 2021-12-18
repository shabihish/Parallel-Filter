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

    void applySepia();

    Pixel &operator=(const Pixel &pixel);

    Pixel operator+(const Pixel &pixel);

    Pixel operator/(const double dividend);
};

vector<vector<Pixel>> applySmoothingFilter(vector<vector<Pixel>> image);
vector<vector<Pixel>> applySepiaFilter(vector<vector<Pixel>> image);

#endif
