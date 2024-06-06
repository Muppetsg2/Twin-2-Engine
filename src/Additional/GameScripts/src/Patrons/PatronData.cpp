#include <Patrons/PatronData.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace glm;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE_NN(PatronData, "PatronData")

SO_SERIALIZATION_BEGIN(PatronData, ScriptableObject)
SO_SERIALIZE_FIELD(patronName)
SO_SERIALIZE_FIELD_F(patronMusic, (uint16_t))
SO_SERIALIZE_FIELD(patronDescription)
SO_SERIALIZE_FIELD_F(patronBonus, (uint16_t))
SO_SERIALIZE_FIELD(moneyMultiplier)
SO_SERIALIZE_FIELD(controlMultiplier)
SO_SERIALIZE_FIELD(additionalMoveRange)
SO_SERIALIZE_FIELD(additionalAbilitiesRange)
SO_SERIALIZE_FIELD(abilitiesCooldownPercent)
SO_SERIALIZE_FIELD(abilitiesPriceDiscount)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(PatronData, ScriptableObject)
SO_DESERIALIZE_FIELD(patronName)
SO_DESERIALIZE_FIELD_F_T(patronMusic, (PatronMusic), uint16_t)
SO_DESERIALIZE_FIELD(patronDescription)
SO_DESERIALIZE_FIELD_F_T(patronBonus, (PatronBonus), uint16_t)
SO_DESERIALIZE_FIELD(moneyMultiplier)
SO_DESERIALIZE_FIELD(controlMultiplier)
SO_DESERIALIZE_FIELD(additionalMoveRange)
SO_DESERIALIZE_FIELD(additionalAbilitiesRange)
SO_DESERIALIZE_FIELD(abilitiesCooldownPercent)
SO_DESERIALIZE_FIELD(abilitiesPriceDiscount)
SO_DESERIALIZATION_END()


// Getters
std::string PatronData::GetPatronName() const {
    return patronName;
}

std::string PatronData::GetPatronDescription() const {
    return patronDescription;
}

PatronBonus PatronData::GetPatronBonus() const {
    return patronBonus;
}

float PatronData::GetBonus() const {
    switch (patronBonus) {
    case PatronBonus::MONEY_GAIN:
        return moneyMultiplier;
    case PatronBonus::CONTROL_MULTIPLIER:
        return controlMultiplier;
    case PatronBonus::MOVE_RANGE:
        return additionalMoveRange;
    case PatronBonus::ABILITIES_RANGE:
        return additionalAbilitiesRange;
    case PatronBonus::ABILITIES_COOLDOWN:
        return abilitiesCooldownPercent;
    case PatronBonus::ABILITIES_PRICE:
        return 100.0f - abilitiesPriceDiscount;
    default:
        return 1.0f;
    }
}

std::string PatronData::GetBonusAsString() const {
    switch (patronBonus) {
    case PatronBonus::MONEY_GAIN:
        return "Money Gain\nBonus: x" + std::to_string(moneyMultiplier);
    case PatronBonus::CONTROL_MULTIPLIER:
        return "Tile Taking\nBonus: x" + std::to_string(controlMultiplier);
    case PatronBonus::MOVE_RANGE:
        return "Move Range\nBonus: +" + std::to_string(additionalMoveRange);
    case PatronBonus::ABILITIES_RANGE:
        return "Abilities Range\nBonus: +" + std::to_string(additionalAbilitiesRange);
    case PatronBonus::ABILITIES_COOLDOWN:
        return "Abilities Cooldown Reduce\nBonus: -" + std::to_string(static_cast<int>(abilitiesCooldownPercent)) + "%";
    case PatronBonus::ABILITIES_PRICE:
        return "Abilities Price Discount\nBonus: -" + std::to_string(static_cast<int>(abilitiesPriceDiscount)) + "%";
    default:
        return "";
    }
}

float PatronData::GetMoneyMultiplier() const
{
    return moneyMultiplier;
}

float PatronData::GetControlMultiplier() const
{
    return controlMultiplier;
}

float PatronData::GetAdditionalMoveRange() const
{
    return additionalMoveRange;
}

float PatronData::GetAdditionalAbilitiesRange() const
{
    return additionalAbilitiesRange;
}

float PatronData::GetAbilitiesCooldownPercent() const
{
    return abilitiesCooldownPercent;
}

float PatronData::GetAbilitiesPriceDiscount() const
{
    return abilitiesPriceDiscount;
}

// Setters
void PatronData::SetPatronName(const std::string& name) {
    patronName = name;
}

void PatronData::SetPatronDescription(const std::string& description) {
    patronDescription = description;
}

void PatronData::SetPatronBonus(PatronBonus bonus) {
    patronBonus = bonus;
}
