#pragma once
#include <core/Component.h>
#include <ui/Text.h>
#include <ui/Button.h>

class GameOverManager : public Twin2Engine::Core::Component {
public:
	static GameOverManager* instance;

private:
	Twin2Engine::Core::GameObject* _gameOverCanvas = nullptr;

	Twin2Engine::UI::Text* _timeText;
	Twin2Engine::UI::Text* _albumsNumText;
	Twin2Engine::UI::Text* _concertsNumText;
	Twin2Engine::UI::Text* _fansNumText;

	Twin2Engine::UI::Button* _newGameButton;
	Twin2Engine::UI::Button* _menuButton;

	int _gameOverCanvasActiveChangeEvent = -1;
	int _newGameButtonEvent = -1;
	int _menuButtonEvent = -1;

	void NewGame();
	void Menu();

	void UpdateTexts();

	void InitializeButtons();
	void InitializeGameOverCanvas();

public:
	void Open();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;
	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};