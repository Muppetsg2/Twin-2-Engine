#pragma once

#include <core/Component.h>

#include <Abilities/ConcertAbilityController.h>

#include <UI/Text.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;

class ConcertRoadAnimation : public Twin2Engine::Core::Component {
private:

	ConcertAbilityController* _ability = nullptr;

	Twin2Engine::UI::Text* _countText = nullptr;
	Twin2Engine::Core::GameObject* _countObj = nullptr;
	Twin2Engine::UI::Text* _explenationText = nullptr;
	Twin2Engine::UI::Text* _timeText = nullptr;

	bool _start = false;
	float _startTimeFactor = .3f;
	float _startAnimCounter = 0.f;

	bool _exit = false;
	float _exitTimeFactor = .05f;
	float _exitAnimCounter = 0.f;

	float _exitWaitTime = 2.5f;

	int _endValue = 0;
	int _numOfPoints = 3;

public:

	void Start(ConcertAbilityController* a);
	void Exit();

	virtual void Initialize() override;

	virtual void Update() override;

	virtual void OnDestroy() override;

	virtual YAML::Node Serialize() const override;

	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};