#ifndef PATRON_DATA_H
#define PATRON_DATA_H

#include <core/ScriptableObject.h>

enum class PatronBonus {
    MoneyGain = 0,
    ControlMultiplier,
    MoveRange,
    AbilitiesRange,
    AbilitiesCooldown
};

enum class PatronMusic {
    Rock = 0,
    Electronic,
    Pop,
    Jazz,
    Disco
};

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

#endif // PATRON_DATA_H
