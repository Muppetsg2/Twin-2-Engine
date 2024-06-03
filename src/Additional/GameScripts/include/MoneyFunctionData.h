#pragma once

#include <core/ScriptableObject.h>

class MoneyFunctionData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(MoneyFunctionData);

    float Equation(float x, float start = 0.0f);
public:
    float flatness = 10.0f;

    bool hasXLimits = false;

    float minX = 0.0f;
    float maxX = 100.0f;

    bool hasYLimits = false;

    float minY = -10.0f;
    float maxY = 10.0f;

    float GetValue(float x, float start = 0.0f);

    SO_SERIALIZE();
    SO_DESERIALIZE();

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(MoneyFunctionData);