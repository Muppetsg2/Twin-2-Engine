#include <core/MathExtensions.h>

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd)
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}

double mod(double val1, double val2)
{
    if (val1 < 0 && val2 <= 0) {
        return 0;
    }

    double x = val1 / val2;
    double z = std::floor(val1 / val2);
    return (x - z) * val2;
}

float lerpf(float a, float b, float f)
{
    return a + f * (b - a);
}
