#pragma once

#include <core/ScriptableObject.h>

ENUM_CLASS_BASE_VALUE(PatronBonus, uint8_t, MONEY_GAIN, 0, CONTROL_MULTIPLIER, 1, MOVE_RANGE, 2, ABILITIES_RANGE, 3, ABILITIES_COOLDOWN, 4, ABILITIES_PRICE, 5);

ENUM_CLASS_BASE_VALUE(PatronMusic, uint8_t, ROCK, 0, ELECTRONIC, 1, POP, 2, JAZZ, 3, DISCO, 4, HEAVY_METAL, 5);

class PatronData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(PatronData)

public:
    std::string patronName;
    PatronMusic patronMusic;
    std::string patronDescription;
    PatronBonus patronBonus;

    float moneyMultiplier = 1.0f;
    float controlMultiplier = 1.0f;
    float additionalMoveRange = 0.0f;
    float additionalAbilitiesRange = 0.0f;
    float abilitiesCooldownPercent = 50.0f;
    float abilitiesPriceDiscount = 30.f;


    // Getters
    std::string GetPatronName() const;
    std::string GetPatronDescription() const;
    PatronBonus GetPatronBonus() const;
    float GetBonus() const;
    std::string GetBonusAsString() const;

    // Setters
    void SetPatronName(const std::string& name);
    void SetPatronDescription(const std::string& description);
    void SetPatronBonus(PatronBonus bonus);

    SO_SERIALIZE()
    SO_DESERIALIZE()
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(PatronData);