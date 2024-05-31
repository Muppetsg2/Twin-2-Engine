#include <MoneyFunctionData.h>

using namespace Twin2Engine::Core;
using namespace glm;
using namespace std;


SCRIPTABLE_OBJECT_SOURCE_CODE_NN(MoneyFunctionData, "MoneyFunctionData")

SO_SERIALIZATION_BEGIN(MoneyFunctionData, ScriptableObject)
SO_SERIALIZE_FIELD(flatness)
SO_SERIALIZE_FIELD(hasXLimits)
SO_SERIALIZE_FIELD(minX)
SO_SERIALIZE_FIELD(maxX)
SO_SERIALIZE_FIELD(hasYLimits)
SO_SERIALIZE_FIELD(minY)
SO_SERIALIZE_FIELD(maxY)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(MoneyFunctionData, ScriptableObject)
SO_DESERIALIZE_FIELD(flatness)
SO_DESERIALIZE_FIELD(hasXLimits)
SO_DESERIALIZE_FIELD(minX)
SO_DESERIALIZE_FIELD(maxX)
SO_DESERIALIZE_FIELD(hasYLimits)
SO_DESERIALIZE_FIELD(minY)
SO_DESERIALIZE_FIELD(maxY)
SO_DESERIALIZATION_END()
float MoneyFunctionData::Equation(float x, float start)
{
    return glm::exp(x / flatness) + start - 1.0f;
}

float MoneyFunctionData::GetValue(float x, float start)
{
    float y = Equation(hasXLimits ? glm::clamp(x, minX, maxX) : x, start);
    return hasYLimits ? glm::clamp(y, minY, maxY) : y;
}