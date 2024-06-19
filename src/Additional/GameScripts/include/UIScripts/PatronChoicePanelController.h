#pragma once

// CORE
#include <core/Component.h>
#include <manager/SceneManager.h>

// UI
#include <ui/Button.h>

// GAME
#include <Patrons/PatronData.h>
#include <GameManager.h>


class PatronChoicePanelController : public Twin2Engine::Core::Component
{
    std::vector<PatronData*> _patrons;
    std::vector<Twin2Engine::UI::Button*> _patronsButtons;

    bool choosed = false;

    void Choose(PatronData* patron);

public:
    void StartChoose();
    void StopChoose();

    bool IsChoosed();

    virtual void Initialize() override;

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};