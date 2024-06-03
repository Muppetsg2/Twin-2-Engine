#include <Abilities/ConcertAbilityController.h>

using namespace Twin2Engine::Core;

void ConcertAbilityController::Initialize() {
    playable = GetGameObject()->GetComponent<Playable>();
    money = GetGameObject()->GetComponent<MoneyGainFromTiles>();

    startMoneyRequired = moneyRequired;

    //UIControllerConcertAbility::Instance->OnUseAbilityClicked = std::bind(&ConcertAbilityController::Use, this);

    //if (playable->patron->patronBonus == PatronBonus::AbilitiesCooldown) {
    //    cooldownTime *= playable->patron->GetBonus() / 100.0f;
    //}
}

void ConcertAbilityController::Update() {
    if (!playable->OwnTiles.empty()) {
        //moneyRequired = moneyFunction->GetValue(playable->OwnTiles.size() - 1, startMoneyRequired);
    }

    //UIControllerConcertAbility::Instance->text.text = "Concert\n" + std::to_string(moneyRequired) + "$";

    if (currTimerTime > 0.0f) {
        currTimerTime -= Time::GetDeltaTime();
        if (currTimerTime < 0.0f) {
            currTimerTime = 0.0f;
            StopPerformingConcert();
        }

        //if (coroutineCooldown) {
        //    UIControllerConcertAbility::Instance->timerText.text = "Cooldown: " + std::to_string(static_cast<int>(currTimerTime)) + " s";
        //}
        //else {
        //    UIControllerConcertAbility::Instance->timerText.text = std::to_string(static_cast<int>(currTimerTime)) + " s";
        //}
        //UIControllerConcertAbility::Instance->button.interactable = false;
        //UIControllerConcertAbility::Instance->timerText.gameObject.SetActive(true);
    }
    //else {
    //    UIControllerConcertAbility::Instance->timerText.gameObject.SetActive(false);
    //    if (money->money < moneyRequired) {
    //        UIControllerConcertAbility::Instance->button.interactable = false;
    //    }
    //    else {
    //        UIControllerConcertAbility::Instance->button.interactable = true;
    //        if (Input::GetKey(KeyCode::X)) {
    //            Use();
    //        }
    //    }
    //}
    if (currCooldown > 0.0f)
    {
        currCooldown -= Time::GetDeltaTime();
        if (currCooldown <= 0.0f)
        {
            canUse = true;
        }
    }

    if (Input::IsKeyPressed(KEY::C))
    {
        SPDLOG_INFO("Using Concert");
        Use();
    }

}

void ConcertAbilityController::Use() 
{
    //if (canUse && money->SpendMoney(moneyRequired)) 
    {
        StartPerformingConcert();

        //UIControllerConcertAbility::Instance->button.interactable = false;
        currTimerTime = lastingTime;
    }
}

void ConcertAbilityController::StartCooldown() 
{
    currCooldown = cooldownTime;
}

void ConcertAbilityController::CreateCooldownCoroutine() {
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(cooldownTime)));

    canUse = true;
    //coroutineCooldown = nullptr;
    //UIControllerConcertAbility::Instance->button.interactable = true;
}

void ConcertAbilityController::StartPerformingConcert() 
{
    canUse = false;
    playable->concertUsed++;
    //if (coroutinePerformingConcert) {
    //    StopCoroutine(coroutinePerformingConcert);
    //}

    savedTakingOverSpeed = playable->TakeOverSpeed;
    playable->TakeOverSpeed = takingOverSpeed;

    //coroutinePerformingConcert = std::async(std::launch::async, &ConcertAbilityController::CreatePerformingConcertCoroutine, this);
}

void ConcertAbilityController::StopPerformingConcert()
{
    //coroutinePerformingConcert = nullptr;
    playable->TakeOverSpeed = savedTakingOverSpeed;

    StartCooldown();
}



YAML::Node ConcertAbilityController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["lastingTime"] = lastingTime;
    node["cooldownTime"] = cooldownTime;
    node["moneyRequired"] = moneyRequired;
    node["takingOverSpeed"] = takingOverSpeed;

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
        DrawInheritedFields();
    }
}
#endif