#include <GameTimer.h>
#include <GameManager.h> // Assuming GameManager is defined somewhere

int GameTimer::highestDaysCounter = 0;
int GameTimer::highestWeeksCounter = 0;
int GameTimer::highestMonthsCounter = 0;
int GameTimer::highestYearsCounter = 0;

GameTimer* GameTimer::instance = nullptr;

GameTimer* GameTimer::Instance() {
    if (instance == nullptr) {
        instance = new GameTimer();
    }
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

    SetEnable(false);
}

void GameTimer::Update(float deltaTime) {
    if (!GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
        secondsCounter += deltaTime;
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
            
            OnDayTicked(daysCounter);
        }
    }
}

void GameTimer::ResetTimer() {
    daysCounter = 1;
    weeksCounter = 1;
    monthsCounter = 1;
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
