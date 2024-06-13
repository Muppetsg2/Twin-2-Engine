#include <Abilities/ConcertAbilityController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

void ConcertAbilityController::Initialize() {
    playable = GetGameObject()->GetComponent<Playable>();
    money = GetGameObject()->GetComponent<MoneyGainFromTiles>();
    usedMoneyRequired = moneyRequired;
}

void ConcertAbilityController::OnDestroy() {

}

void ConcertAbilityController::Update() {
    if (!playable->OwnTiles.empty()) {
        usedMoneyRequired = moneyFunction->GetValue(playable->OwnTiles.size() - 1, moneyRequired);
    }

    if (currTimerTime > 0.0f) {
        currTimerTime -= Time::GetDeltaTime();
        if (currTimerTime < 0.0f) {
            currTimerTime = 0.0f;
            StopPerformingConcert();
        }
    }
    if (currCooldown > 0.0f)
    {
        currCooldown -= Time::GetDeltaTime();
        if (currCooldown <= 0.0f)
        {
            currCooldown = 0.0f;
            canUse = true;
            OnEventAbilityCooldownFinished.Invoke(playable);
        }
    }

    if (Input::IsKeyPressed(KEY::C))
    {
        SPDLOG_INFO("Using Concert");
        Use();
    }

}

bool ConcertAbilityController::Use() 
{
    SPDLOG_INFO("Trying use ConcertAbility");
    if (canUse && money->SpendMoney(GetCost()))
    {
        SPDLOG_INFO("Using ConcertAbility");

        StartPerformingConcert();
        OnEventAbilityStarted.Invoke(playable);
        return true;
    }
    return false;
}

void ConcertAbilityController::StartCooldown() 
{
    currCooldown = cooldownTime;
    if (playable->patron->GetPatronBonus() == PatronBonus::ABILITIES_COOLDOWN) {
        currCooldown *= playable->patron->GetBonus() / 100.0f;
    }
    OnEventAbilityCooldownStarted.Invoke(playable);
}

void ConcertAbilityController::StartPerformingConcert() 
{
    canUse = false;
    playable->concertUsed++;
    currTimerTime = lastingTime;

    savedTakingOverSpeed = playable->TakeOverSpeed;
    playable->TakeOverSpeed = takingOverSpeed;
}

void ConcertAbilityController::StopPerformingConcert()
{
    playable->TakeOverSpeed = savedTakingOverSpeed;

    OnEventAbilityFinished.Invoke(playable);

    StartCooldown();
}

float ConcertAbilityController::GetCost() const
{
    if (playable->patron->GetPatronBonus() == PatronBonus::ABILITIES_PRICE) {
        return usedMoneyRequired * (1.0f - playable->patron->GetBonus() / 100.0f);
    }
    return usedMoneyRequired;
}

float ConcertAbilityController::GetAbilityRemainingTime() const
{
    return currTimerTime;
}

float ConcertAbilityController::GetCooldownRemainingTime() const
{
    return currCooldown;
}

float ConcertAbilityController::GetCooldown() const
{
    float tempCooldown = cooldownTime;
    if (playable->patron->GetPatronBonus() == PatronBonus::ABILITIES_COOLDOWN) {
        tempCooldown *= playable->patron->GetBonus() / 100.0f;
    }
    return tempCooldown;
}

YAML::Node ConcertAbilityController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "ConcertAbilityController";
    node["lastingTime"] = lastingTime;
    node["cooldownTime"] = cooldownTime;
    node["moneyRequired"] = moneyRequired;
    node["takingOverSpeed"] = takingOverSpeed;
    node["moneyFunction"] = ScriptableObjectManager::GetPath(moneyFunction->GetId());

    return node;
}

bool ConcertAbilityController::Deserialize(const YAML::Node& node)
{
    if (!node["lastingTime"] && !node["cooldownTime"] && !node["moneyRequired"] && !node["takingOverSpeed"] && !Component::Deserialize(node))
        return false;

    lastingTime = node["lastingTime"].as<float>();
    cooldownTime = node["cooldownTime"].as<float>();
    moneyRequired = node["moneyRequired"].as<float>();
    takingOverSpeed = node["takingOverSpeed"].as<float>();
    moneyFunction = static_cast<MoneyFunctionData*>(ScriptableObjectManager::Load(node["moneyFunction"].as<string>()));

    return true;
}

#if _DEBUG
bool ConcertAbilityController::DrawInheritedFields()
{
    if (Twin2Engine::Core::Component::DrawInheritedFields()) return true;

    string id = string(std::to_string(this->GetId()));

    ImGui::InputFloat(string("LastingTime##").append(id).c_str(), &lastingTime);
    ImGui::InputFloat(string("CooldownTime##").append(id).c_str(), &cooldownTime);
    ImGui::InputFloat(string("MoneyRequired##").append(id).c_str(), &moneyRequired);
    ImGui::InputFloat(string("TakingOverSpeed##").append(id).c_str(), &takingOverSpeed);

    return false;
}

void ConcertAbilityController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("ConcertAbilityController##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
        if (DrawInheritedFields()) return;
    }
}
#endif