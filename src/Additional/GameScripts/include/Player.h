#pragma once 

#include <core/GameObject.h>

#include <Playable.h>
#include <PlayerMovement.h>
#include <GameManager.h>
#include <Patrons/PatronData.h>


#include <core/Input.h>

//using namespace Twin2Engine::Core;

//class Playable;
class PlayerMovement;

class Player : public Playable {
private:
    //static Mesh* hexMesh;
    Twin2Engine::Core::GameObject* hexIndicator;
    //TextMeshProUGUI* albumTimer;
    //Button* albumButton;
    //TextMeshProUGUI* fansTimer;
    //Button* fansButton;
    PlayerMovement* move;
    bool lost;

    Playable* fightingPlayable = nullptr;
    //Coroutine* fansCorountine;

public:

    virtual void Initialize() override;
    virtual void Update() override;

    void AlbumCall();
    void FansCall();
    void FansControlDraw();
    void StartMove(HexTile* tile);
    void FinishMove(HexTile* tile);
    void MinigameEnd();
    //void MinigameEnd(void* sender);
    //void StartPaperRockScisors(Playable* playable);
    //void WonPaperRockScisors(Playable* playable);
    //void LostPaperRockScisors(Playable* playable);
    //void WonFansControl(Playable* playable);
    //void StartFansControl(Playable* playable);
    //void LostFansControl(Playable* playable);

    virtual void LostPaperRockScissors(Playable* playable) override;
    virtual void WonPaperRockScissors(Playable* playable) override;
    virtual void LostFansControl(Playable* playable) override;
    virtual void WonFansControl(Playable* playable) override;
    virtual void StartPaperRockScissors(Playable* playable) override;
    virtual void StartFansControl(Playable* playable) override;

protected:
    virtual void OnDead() override;


public:
	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};