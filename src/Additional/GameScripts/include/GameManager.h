#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <vector>
//#include <pch.h>

using namespace Twin2Engine::Core;

class Playable;

//class ConcertRoad : public Component {
class GameManager : public Component {
public:
    static GameManager* instance;

    std::vector<Playable*> entities;

    virtual void Initialize() override;
    virtual void Update() override;
};