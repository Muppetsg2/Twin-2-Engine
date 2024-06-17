#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

ENUM_CLASS_BASE(EaseFunction, uint8_t, LINE, SINE, CUBIC, QUINT, CIRC, ELASTIC, QUAD, QUART, EXPO, BACK, BOUNCE)
ENUM_CLASS_BASE(EaseFunctionType, uint8_t, IN_F, OUT_F, IN_OUT_F)

// SINE
static float easeInSine(float x)
{
    return 1.f - cosf((x * M_PI) / 2.f);
}

static float easeOutSine(float x)
{
    return sinf((x * M_PI) / 2.f);
}

static float easeInOutSine(float x)
{
    return -(cosf(M_PI * x) - 1.f) / 2.f;
}

// CUBIC
static float easeInCubic(float x)
{
    return x * x * x;
}

static float easeOutCubic(float x)
{
    float v = (1.f - x);
    return 1.f - (v * v * v);
}

static float easeInOutCubic(float x)
{
    if (x < 0.5f) return 4.f * x * x * x;
    else {
        float v = (-2.f * x + 2.f);
        return 1.f - (v * v * v) / 2.f;
    }
}

// QUINT
static float easeInQuint(float x)
{
    return x * x * x * x * x;
}

static float easeOutQuint(float x)
{
    float v = 1.f - x;
    return 1.f - (v * v * v * v * v);
}

static float easeInOutQuint(float x)
{
    if (x < 0.5f) return 16.f * x * x * x * x * x;
    else {
        float v = -2.f * x + 2.f;
        return 1.f - (v * v * v * v * v) / 2.f;
    }
}

// CIRC
static float easeInCirc(float x)
{
    return 1.f - sqrtf(1.f - (x * x));
}

static float easeOutCirc(float x)
{
    float v = x - 1.f;
    return sqrtf(1.f - (v * v));
}

static float easeInOutCirc(float x)
{
    if (x < 0.5f) return 1.f - sqrtf(1.f - (4.f * x * x)) / 2.f;
    else {
        float v = -2.f * x + 2.f;
        return (sqrtf(1.f - (v * v)) + 1.f) / 2.f;
    }
}

// ELASTIC
static float easeInElastic(float x)
{
    const float c4 = (2.f * M_PI) / 3.f;
    return x == 0.f ? 0.f : x == 1.f ? 1.f : -powf(2.f, 10.f * x - 10.f) * sinf((x * 10.f - 10.75f) * c4);
}

static float easeOutElastic(float x)
{
    const float c4 = (2.f * M_PI) / 3.f;
    return x == 0.f ? 0.f : x == 1.f ? 1.f : powf(2.f, -10.f * x) * sinf((x * 10.f - 0.75f) * c4) + 1.f;
}

static float easeInOutElastic(float x)
{
    const float c5 = (2.f * M_PI) / 4.5f;
    return x == 0.f ? 0.f : x == 1.f ? 1.f : x < 0.5f ? -(powf(2.f, 20.f * x - 10.f) * sinf((20.f * x - 11.125f) * c5)) / 2.f : (powf(2.f, -20.f * x + 10.f) * sinf((20.f * x - 11.125f) * c5)) / 2.f + 1.f;
}

// QUAD
static float easeInQuad(float x)
{
    return x * x;
}

static float easeOutQuad(float x)
{
    float v = (1.f - x);
    return 1.f - (v * v);
}

static float easeInOutQuad(float x)
{
    if (x < 0.5f) return 2.f * x * x;
    else {
        float v = -2.f * x + 2.f;
        return 1.f - (v * v) / 2.f;
    }
}

// QUART
static float easeInQuart(float x)
{
    return x * x * x * x;
}

static float easeOutQuart(float x)
{
    float v = 1.f - x;
    return 1.f - (v * v * v * v);
}

static float easeInOutQuart(float x)
{
    if (x < 0.5f) return 8 * x * x * x * x;
    else {
        float v = -2.f * x + 2.f;
        return 1.f - (v * v * v * v) / 2;
    }
}

// EXPO
static float easeInExpo(float x)
{
    return x == 0.f ? 0.f : powf(2.f, 10.f * x - 10.f);
}

static float easeOutExpo(float x)
{
    return x == 1.f ? 1.f : 1.f - powf(2.f, -10.f * x);
}

static float easeInOutExpo(float x)
{
    return x == 0.f ? 0.f : x == 1.f ? 1.f : x < 0.5f ? powf(2.f, 20.f * x - 10.f) / 2.f : (2.f - powf(2.f, -20.f * x + 10.f)) / 2.f;
}

// BACK
static float easeInBack(float x)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;
    return c3 * x * x * x - c1 * x * x;
}

static float easeOutBack(float x)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;
    float v = x - 1.f;
    return 1.f + c3 * v * v * v + c1 * v * v;
}

static float easeInOutBack(float x)
{
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;
    float v1 = 2.f * x;
    float v2 = 2.f * x - 2.f;
    return x < 0.5f ? (v1 * v1 * ((c2 + 1.f) * v1 - c2)) / 2.f : (v2 * v2 * ((c2 + 1.f) * v2 + c2) + 2.f) / 2.f;
}

// BOUNCE
static float easeOutBounce(float x)
{
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (x < 1.f / d1) return n1 * x * x;
    else if (x < 2.f / d1) return n1 * (x -= 1.5f / d1) * x + 0.75f;
    else if (x < 2.5f / d1) return n1 * (x -= 2.25f / d1) * x + 0.9375f;
    else return n1 * (x -= 2.625f / d1) * x + 0.984375f;
}

static float easeInBounce(float x)
{
    return 1.f - easeOutBounce(1.f - x);
}

static float easeInOutBounce(float x)
{
    return x < 0.5f ? (1.f - easeOutBounce(1.f - 2.f * x)) / 2.f : (1.f + easeOutBounce(2.f * x - 1.f)) / 2.f;
}

static float getEaseFuncValue(EaseFunction func, EaseFunctionType type, float x)
{
    switch (func)
    {
        case EaseFunction::LINE:
        {
            return x;
        }
        case EaseFunction::SINE:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInSine(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutSine(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutSine(x);
                }
            }
        }
        case EaseFunction::CUBIC:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInCubic(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutCubic(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutCubic(x);
                }
            }
        }
        case EaseFunction::QUINT:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInQuint(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutQuint(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutQuint(x);
                }
            }
        }
        case EaseFunction::CIRC:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInCirc(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutCirc(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutCirc(x);
                }
            }
        }
        case EaseFunction::ELASTIC:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInElastic(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutElastic(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutElastic(x);
                }
            }
        }
        case EaseFunction::QUAD:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInQuad(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutQuad(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutQuad(x);
                }
            }
        }
        case EaseFunction::QUART:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInQuart(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutQuart(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutQuart(x);
                }
            }
        }
        case EaseFunction::EXPO:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInExpo(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutExpo(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutExpo(x);
                }
            }
        }
        case EaseFunction::BACK:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInBack(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutBack(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutBack(x);
                }
            }
        }
        case EaseFunction::BOUNCE:
        {
            switch (type)
            {
                case EaseFunctionType::IN_F:
                {
                    return easeInBounce(x);
                }
                case EaseFunctionType::OUT_F:
                {
                    return easeOutBounce(x);
                }
                case EaseFunctionType::IN_OUT_F:
                {
                    return easeInOutBounce(x);
                }
            }
        }
    }

    return x;
}