#pragma once

#include <core/ScriptableObject.h>

#include <AreaTaking/HexTileTextureData.h>

ENUM_CLASS_BASE_VALUE(PatronBonus, uint8_t, MONEY_GAIN, 0, CONTROL_MULTIPLIER, 1, MOVE_RANGE, 2, ABILITIES_RANGE, 3, ABILITIES_COOLDOWN, 4, ABILITIES_PRICE, 5);

ENUM_CLASS_BASE_VALUE(PatronMusic, uint8_t, ROCK, 0, ELECTRONIC, 1, POP, 2, CLASSIC, 3, DISCO, 4, HEAVY_METAL, 5);

class PatronData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(PatronData)

    std::string patronName;
    PatronMusic patronMusic;
    std::string patronDescription;
    PatronBonus patronBonus;

    TILE_COLOR color;
    float moneyMultiplier = 1.0f;
    float controlMultiplier = 1.0f;
    float additionalMoveRange = 0.0f;
    float additionalAbilitiesRange = 0.0f;
    float abilitiesCooldownPercent = 50.0f;
    float abilitiesPriceDiscount = 30.f;
public:


    // Getters
    std::string GetPatronName() const;
    std::string GetPatronDescription() const;
    PatronMusic GetPatronMusic() const;
    PatronBonus GetPatronBonus() const;
    float GetBonus() const;
    std::string GetBonusAsString() const;

    TILE_COLOR GetColor() const;

    float GetMoneyMultiplier() const;
    float GetControlMultiplier() const;
    float GetAdditionalMoveRange() const;
    float GetAdditionalAbilitiesRange() const;
    float GetAbilitiesCooldownPercent() const;
    float GetAbilitiesPriceDiscount() const;

    // Setters
    void SetPatronName(const std::string& name);
    void SetPatronDescription(const std::string& description);
    void SetPatronBonus(PatronBonus bonus);

    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG
//protected:
//    virtual bool DrawInheritedFields() override;
//public:
virtual void DrawEditor() override
    {

        std::string id = std::to_string(GetId());

        ImGui::Text("Name: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text(Twin2Engine::Manager::ScriptableObjectManager::GetName(GetId()).c_str());
        ImGui::PopFont();

        static std::string patronNameTemp;

        ImGui::InputText("PatronName##SO", &patronNameTemp, 255);
        patronName = patronNameTemp;

        ImGui::InputText("PatronDescription##SO", &patronDescription, 1024);

        static PatronBonus selectedPatronBonus;
        if (ImGui::BeginCombo("Patron Bonus##SO", to_string(selectedPatronBonus).c_str()))
        {
            for (uint8_t value = 0; value < ::size<PatronBonus>(); ++value)
            {
                if (ImGui::Selectable(to_string((PatronBonus)value).c_str()))
                {
                    selectedPatronBonus = (PatronBonus)value;
                }
            }

            patronBonus = selectedPatronBonus;
            ImGui::EndCombo();
        }

        static PatronMusic selectedPatronMusic;
        if (ImGui::BeginCombo("Patron Music##SO", to_string(selectedPatronMusic).c_str()))
        {
            for (uint8_t value = 0; value < ::size<PatronMusic>(); ++value)
            {
                if (ImGui::Selectable(to_string((PatronMusic)value).c_str()))
                {
                    selectedPatronMusic = (PatronMusic)value;
                }
            }

            patronMusic = selectedPatronMusic;
            ImGui::EndCombo();
        }

        ImGui::InputFloat(std::string("moneyMultiplier##SO").append(id).c_str(), &moneyMultiplier);
        ImGui::InputFloat(std::string("controlMultiplier##SO").append(id).c_str(), &controlMultiplier);
        ImGui::InputFloat(std::string("additionalMoveRange##SO").append(id).c_str(), &additionalMoveRange);
        ImGui::InputFloat(std::string("additionalAbilitiesRange##SO").append(id).c_str(), &additionalAbilitiesRange);
        ImGui::InputFloat(std::string("abilitiesCooldownPercent##SO").append(id).c_str(), &abilitiesCooldownPercent);
        ImGui::InputFloat(std::string("abilitiesPriceDiscount##SO").append(id).c_str(), &abilitiesPriceDiscount);

        ScriptableObject::DrawInheritedFields();
    }
#endif
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(PatronData);