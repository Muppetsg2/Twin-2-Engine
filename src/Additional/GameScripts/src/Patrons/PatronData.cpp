#include <Patrons/PatronData.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace glm;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE_NN(PatronData, "PatronData")

SO_SERIALIZATION_BEGIN(PatronData, ScriptableObject)
//SO_SERIALIZE_FIELD_F(prefabCity, PrefabManager::GetPrefabPath)
//SO_SERIALIZE_FIELD(byRegions)
//SO_SERIALIZE_FIELD(density)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(PatronData, ScriptableObject)
//SO_DESERIALIZE_FIELD_F_T(prefabCity, PrefabManager::LoadPrefab, string)
//SO_DESERIALIZE_FIELD(byRegions)
//SO_DESERIALIZE_FIELD(density)
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
    case PatronBonus::MoneyGain:
        return moneyMultiplier;
    case PatronBonus::ControlMultiplier:
        return controlMultiplier;
    case PatronBonus::MoveRange:
        return additionalMoveRange;
    case PatronBonus::AbilitiesRange:
        return additionalAbilitiesRange;
    case PatronBonus::AbilitiesCooldown:
        return abilitiesCooldownPercent;
    default:
        return 1.0f;
    }
}

std::string PatronData::GetBonusAsString() const {
    switch (patronBonus) {
    case PatronBonus::MoneyGain:
        return "Money Gain\nBonus: x" + std::to_string(moneyMultiplier);
    case PatronBonus::ControlMultiplier:
        return "Tile Taking\nBonus: x" + std::to_string(controlMultiplier);
    case PatronBonus::MoveRange:
        return "Move Range\nBonus: +" + std::to_string(additionalMoveRange);
    case PatronBonus::AbilitiesRange:
        return "Abilities Range\nBonus: +" + std::to_string(additionalAbilitiesRange);
    case PatronBonus::AbilitiesCooldown:
        return "Abilities Cooldown Reduce\nBonus: -" + std::to_string(static_cast<int>(abilitiesCooldownPercent)) + "%";
    default:
        return "";
    }
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
