#include <AreaTaking/GetMoneyFromTiles.h>

#include <Playable.h>

using namespace Twin2Engine::Core;

void MoneyGainFromTiles::Initialize() {
    playable = GetGameObject()->GetComponent<Playable>();
    eventIdOnDayTicked = GameTimer::Instance()->OnDayTicked += [&](int day) {
        SPDLOG_INFO("Lambda called");
        UpdateMoney(dynamic_cast<Player*>(playable) != nullptr);
        };
}

void MoneyGainFromTiles::Update() {
    // Implement any necessary update logic if needed.
}

void MoneyGainFromTiles::OnDestroy() {
    GameTimer::Instance()->OnDayTicked -= eventIdOnDayTicked;
    // Implement any necessary update logic if needed.
}

float MoneyGainFromTiles::GetTileTypeFactor(Generation::MapHexTile::HexTileType tileType) {

    return 1.0f;
}

float MoneyGainFromTiles::GainMoneyFromTile(HexTile* tile) {
    return moneyBaseFactor * GetTileTypeFactor(tile->GetMapHexTile()->type) * tile->percentage * 0.01f;
}

void MoneyGainFromTiles::UpdateMoney(bool player) {
    SPDLOG_INFO("UpdateMoney called");
    if (GameManager::instance->minigameActive) return;
    SPDLOG_INFO("After condition called");

    float patronMul = 1.0f;

    //if (playable->patron->patronBonus == PatronBonus::MONEY_GAIN) {
    //    patronMul = playable->patron->GetBonus();
    //}

    for (auto* tile : playable->OwnTiles) {
        money += GainMoneyFromTile(tile) * patronMul;
    }

    if (player) {
        SPDLOG_INFO("Money: {}", money);
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

    node["type"] = "MoneyGainFromTiles";
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