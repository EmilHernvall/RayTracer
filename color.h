#ifndef __COLOR_H_
#define __COLOR_H_

#include <cmath>
#include "vec3.h"

class Color : public vec3 {
public:
    Color();
    Color(double r, double g, double b);

    inline int rgb() const {
        double inv_gamma = 1.0/2.2;
        int r = 0xFF * pow(x(), inv_gamma);
        if (r > 0xFF) { r = 0xFF; }

        int g = 0xFF * pow(y(), inv_gamma);
        if (g > 0xFF) { g = 0xFF; }

        int b = 0xFF * pow(z(), inv_gamma);
        if (b > 0xFF) { b = 0xFF; }

        return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    }
};

#endif // __COLOR_H_
