#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <ui/Image.h>


class PopularityGainingBonusBarController : public Twin2Engine::Core::Component
{
	static PopularityGainingBonusBarController* _instance;

	Twin2Engine::UI::Image* _currentBonusBar;
	Twin2Engine::UI::Image* _possibleBonusBar;

	float _maxBonus;
	float _currentBonus;
	float _possibleBonus;

	float _maxLength;
	float _currentBonusCurrentLength;
	float _possibleBonusCurrentLength;


public:

	static PopularityGainingBonusBarController* Instance();

	virtual void Initialize() override;
	virtual void OnDestroy() override;


	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
	virtual bool DrawInheritedFields() override;
public:
	virtual void DrawEditor() override;
#endif
	void SetColor(glm::vec3 color);
	void SetCurrentBonus(float bonus);
	void AddCurrentBonus(float additionalCurrentBonus);
	void RemoveCurrentBonus(float removedCurrentBonus);

	void AddPossibleBonus(float additionalPossibleBonus);
	void SetPossibleBonus(float bonus);
	void RemovePossibleBonus(float removedPossibleBonus);
};