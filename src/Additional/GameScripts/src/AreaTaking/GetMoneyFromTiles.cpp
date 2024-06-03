#include <AreaTaking/GetMoneyFromTiles.h>

#include <Playable.h>

using namespace Twin2Engine::Core;

void MoneyGainFromTiles::OnEnable() {
    playable = GetGameObject()->GetComponent<Playable>();
}

void MoneyGainFromTiles::Initialize() {
    GameTimer::Instance()->OnDayTicked += [this](int day) {
        UpdateMoney(dynamic_cast<Player*>(playable) != nullptr);
        };
}

void MoneyGainFromTiles::Update() {
    // Implement any necessary update logic if needed.
}

float MoneyGainFromTiles::GetTileTypeFactor(Generation::MapHexTile::HexTileType tileType) {

    return 1.0f;
}

float MoneyGainFromTiles::GainMoneyFromTile(HexTile* tile) {
    return moneyBaseFactor * GetTileTypeFactor(tile->GetMapHexTile()->type) * tile->percentage * 0.01f;
}

void MoneyGainFromTiles::UpdateMoney(bool player) {
    if (GameManager::instance->minigameActive) return;

    float patronMul = 1.0f;

    if (playable->patron->patronBonus == PatronBonus::MONEY_GAIN) {
        patronMul = playable->patron->GetBonus();
    }

    for (auto* tile : playable->OwnTiles) {
        money += GainMoneyFromTile(tile) * patronMul;
    }

    if (player) {
        //MoneyPanelController::Instance().SetMoney(money);
    }
}

bool MoneyGainFromTiles::SpendMoney(float amount) {
    if (amount >= 0 && amount <= money) {
        money -= amount;
        return true;
    }

    //MoneyPanelController::Instance().SetMoney(money);
    return false;
}


YAML::Node MoneyGainFromTiles::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["gainingInterval"] = gainingInterval;
    node["moneyBaseFactor"] = moneyBaseFactor;

    return node;
}

bool MoneyGainFromTiles::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;


    gainingInterval = node["gainingInterval"].as<float>();
    moneyBaseFactor = node["moneyBaseFactor"].as<float>();

    return true;
}

#if _DEBUG

void MoneyGainFromTiles::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("Get Money From Tiles##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        if (Component::DrawInheritedFields()) return;
    }
}

#endif