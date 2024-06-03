#pragma once

#include <core/Component.h>
#include <tools/EventHandler.h>

class GameTimer : public Twin2Engine::Core::Component {
private:
    int secondsPerDay = 1;
    int daysPerWeek = 7;
    int weeksPerMonth = 4;
    int monthsPerYear = 12;

    float secondsCounter = 0;
    int daysCounter = 1;
    int weeksCounter = 1;
    int monthsCounter = 1;
    int yearsCounter = 0;

    static int highestDaysCounter;
    static int highestWeeksCounter;
    static int highestMonthsCounter;
    static int highestYearsCounter;

    static GameTimer* instance;
public:
    static GameTimer* Instance();

    float GetSeconds() const;
    int GetDays() const;
    int GetWeeks() const;
    int GetMonths() const;
    int GetYears() const;
    int GetHighestDays() const;
    int GetHighestWeeks() const;
    int GetHighestMonths() const;
    int GetHighestYears() const;

    float GetTotalSeconds() const;

    void Initialize();
    void Update(float deltaTime);
    void ResetTimer();
    void StartTimer();
    void StopTimer();
    void SaveIfHighest();

    // Event handlers
    Twin2Engine::Tools::EventHandler<int> OnDayTicked;
    Twin2Engine::Tools::EventHandler<int> OnWeekTicked;
    Twin2Engine::Tools::EventHandler<int> OnMonthTicked;
    Twin2Engine::Tools::EventHandler<int> OnYearTicked;

};