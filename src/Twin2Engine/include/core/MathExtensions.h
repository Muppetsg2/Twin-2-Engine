#pragma once

static float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd)
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}

static double mod(double val1, double val2)
{
    if (val1 < 0 && val2 <= 0) {
        return 0;
    }

    double x = val1 / val2;
    double z = std::floor(val1 / val2);
    return (x - z) * val2;
}

static float lerpf(float a, float b, float f)
{
    return a + f * (b - a);
}

static float normpdf(float x, float sigma) {
    return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}