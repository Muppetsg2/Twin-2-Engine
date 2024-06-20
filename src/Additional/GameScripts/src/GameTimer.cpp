#include <GameTimer.h>
#include <GameManager.h> // Assuming GameManager is defined somewhere

int GameTimer::highestDaysCounter = 0;
int GameTimer::highestWeeksCounter = 0;
int GameTimer::highestMonthsCounter = 0;
int GameTimer::highestYearsCounter = 0;

GameTimer* GameTimer::instance = nullptr;

GameTimer* GameTimer::Instance() {
    return instance;
}

float GameTimer::GetSeconds() const { return secondsCounter; }
int GameTimer::GetDays() const { return daysCounter; }
int GameTimer::GetWeeks() const { return weeksCounter; }
int GameTimer::GetMonths() const { return monthsCounter; }
int GameTimer::GetYears() const { return yearsCounter; }
int GameTimer::GetHighestDays() const { return highestDaysCounter; }
int GameTimer::GetHighestWeeks() const { return highestWeeksCounter; }
int GameTimer::GetHighestMonths() const { return highestMonthsCounter; }
int GameTimer::GetHighestYears() const { return highestYearsCounter; }

float GameTimer::GetTotalSeconds() const {
    return secondsCounter + (daysCounter + (weeksCounter + (monthsCounter + yearsCounter * monthsPerYear) * weeksPerMonth) * daysPerWeek) * secondsPerDay;
}

void GameTimer::Initialize() {

    if (instance == nullptr) {
        instance = this;
    }

    //OnDayTicked.RemoveAllCallbacks();
    //OnWeekTicked.RemoveAllCallbacks();
    //OnMonthTicked.RemoveAllCallbacks();
    //OnYearTicked.RemoveAllCallbacks();

    SetEnable(false);
}

void GameTimer::OnDestroy() {
    if (this == instance)
    {
        instance = nullptr;
    }

    //OnDayTicked.RemoveAllCallbacks();
    //OnWeekTicked.RemoveAllCallbacks();
    //OnMonthTicked.RemoveAllCallbacks();
    //OnYearTicked.RemoveAllCallbacks();
}

void GameTimer::Update() {
    if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
        secondsCounter += Time::GetDeltaTime();
        if (secondsCounter >= secondsPerDay) {
            secondsCounter -= secondsPerDay;
            daysCounter++;
            if (daysCounter > daysPerWeek) {
                daysCounter -= daysPerWeek;
                weeksCounter++;
                if (weeksCounter > weeksPerMonth) {
                    weeksCounter -= weeksPerMonth;
                    monthsCounter++;
                    if (monthsCounter > monthsPerYear) {
                        monthsCounter -= monthsPerYear;
                        yearsCounter++;

                        OnYearTicked(yearsCounter);
                    }
                    
                    OnMonthTicked(monthsCounter);
                }
                
                OnWeekTicked(weeksCounter);
            }
            SPDLOG_INFO("Callng OnDayTicked");
            OnDayTicked(daysCounter);
            OnDateTicked(daysCounter, monthsCounter, yearsCounter);
        }
    }
}

void GameTimer::ResetTimer() {
    daysCounter = 0;
    weeksCounter = 0;
    monthsCounter = 0;
    yearsCounter = 0;
}

void GameTimer::StartTimer() {
    SetEnable(true);
}

void GameTimer::StopTimer() {
    SetEnable(false);
}

void GameTimer::SaveIfHighest() {
    if (yearsCounter > highestYearsCounter ||
        (yearsCounter == highestYearsCounter && monthsCounter - 1 > highestMonthsCounter) ||
        (yearsCounter == highestYearsCounter && monthsCounter - 1 == highestMonthsCounter && weeksCounter - 1 > highestWeeksCounter) ||
        (yearsCounter == highestYearsCounter && monthsCounter == highestMonthsCounter && weeksCounter == highestWeeksCounter && daysCounter - 1 == highestDaysCounter)) {

        highestDaysCounter = daysCounter - 1;
        highestWeeksCounter = weeksCounter - 1;
        highestMonthsCounter = monthsCounter - 1;
        highestYearsCounter = yearsCounter;
    }
}

YAML::Node GameTimer::Serialize() const
{
    YAML::Node node = Component::Serialize();
    
    node["type"] = "GameTimer";
    node["secondsPerDay"] = secondsPerDay;
    node["daysPerWeek"] = daysPerWeek;
    node["weeksPerMonth"] = weeksPerMonth;
    node["monthsPerYear"] = monthsPerYear;

    return node;
}

bool GameTimer::Deserialize(const YAML::Node& node)
{
    if (!node["secondsPerDay"] && !node["daysPerWeek"] && !node["weeksPerMonth"] && !node["monthsPerYear"] && !Component::Deserialize(node))
        return false;

    secondsPerDay = node["secondsPerDay"].as<int>();
    daysPerWeek = node["daysPerWeek"].as<int>();
    weeksPerMonth = node["weeksPerMonth"].as<int>();
    monthsPerYear = node["monthsPerYear"].as<int>();

    return true;
}

#if _DEBUG
void GameTimer::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("GameTimer##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
        if (Twin2Engine::Core::Component::DrawInheritedFields()) return;

        string id = string(std::to_string(this->GetId()));

        ImGui::InputInt(string("SecondsPerDay##").append(id).c_str(), &secondsPerDay);
        ImGui::InputInt(string("DaysPerWeek##").append(id).c_str(), &daysPerWeek);
        ImGui::InputInt(string("WeeksPerMonth##").append(id).c_str(), &weeksPerMonth);
        ImGui::InputInt(string("MonthsPerYear##").append(id).c_str(), &monthsPerYear);

        ImGui::Text("secondsCounter: %d", secondsCounter);
        ImGui::Text("daysCounter: %d", daysCounter);
        ImGui::Text("weeksCounter: %d", weeksCounter);
        ImGui::Text("monthsCounter: %d", monthsCounter);
        ImGui::Text("yearsCounter: %d", yearsCounter);
    }
}
#endif