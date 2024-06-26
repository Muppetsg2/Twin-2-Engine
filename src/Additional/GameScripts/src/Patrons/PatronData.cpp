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
SO_SERIALIZE_FIELD_F(color, (uint8_t))
SO_SERIALIZE_FIELD(soundDown)
SO_SERIALIZE_FIELD(soundLeft)
SO_SERIALIZE_FIELD(soundRight)
SO_SERIALIZE_FIELD(soundUp)
SO_SERIALIZE_FIELD(soundDownFalse)
SO_SERIALIZE_FIELD(soundLeftFalse)
SO_SERIALIZE_FIELD(soundRightFalse)
SO_SERIALIZE_FIELD(soundUpFalse)
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
SO_DESERIALIZE_FIELD_F_T(color, (TILE_COLOR), uint8_t)
SO_DESERIALIZE_FIELD(soundDown)
SO_DESERIALIZE_FIELD(soundLeft)
SO_DESERIALIZE_FIELD(soundRight)
SO_DESERIALIZE_FIELD(soundUp)
SO_DESERIALIZE_FIELD(soundDownFalse)
SO_DESERIALIZE_FIELD(soundLeftFalse)
SO_DESERIALIZE_FIELD(soundRightFalse)
SO_DESERIALIZE_FIELD(soundUpFalse)
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

PatronMusic PatronData::GetPatronMusic() const
{
    return patronMusic;
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
        return std::vformat(std::string("Money Gain\nBonus: x{:.2f}"), std::make_format_args(moneyMultiplier));
    case PatronBonus::CONTROL_MULTIPLIER:
        return std::vformat(std::string("Tile Taking\nBonus: x{:.2f}"), std::make_format_args(controlMultiplier));
    case PatronBonus::MOVE_RANGE:
        return std::vformat(std::string("Move Range\nBonus: +{:.2f}"), std::make_format_args(additionalMoveRange));
    case PatronBonus::ABILITIES_RANGE:
        return std::vformat(std::string("Abilities Range\nBonus: +{:.2f}"), std::make_format_args(additionalAbilitiesRange));
    case PatronBonus::ABILITIES_COOLDOWN:
        return std::vformat(std::string("Abilities Cooldown Reduce\nBonus: -{:.2f}%"), std::make_format_args(abilitiesCooldownPercent));
    case PatronBonus::ABILITIES_PRICE:
        return std::vformat(std::string("Abilities Price Discount\nBonus: -{:.2f}%"), std::make_format_args(abilitiesPriceDiscount));
    default:
        return "";
    }
}

TILE_COLOR PatronData::GetColor() const
{
    return color;
}

std::string PatronData::GetSoundDown() const
{
    return soundDown;
}

std::string PatronData::GetSoundLeft() const
{
    return soundLeft;
}

std::string PatronData::GetSoundRight() const
{
    return soundRight;
}

std::string PatronData::GetSoundUp() const
{
    return soundUp;
}

std::string PatronData::GetSoundDownFalse() const
{
    return soundDownFalse;
}

std::string PatronData::GetSoundLeftFalse() const
{
    return soundLeftFalse;
}

std::string PatronData::GetSoundRightFalse() const
{
    return soundRightFalse;
}

std::string PatronData::GetSoundUpFalse() const
{
    return soundUpFalse;
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
