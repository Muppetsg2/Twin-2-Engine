#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <unordered_set>
#include <vector>
#include <Generation/MapHexTile.h>
//#include <pch.h>

using namespace Twin2Engine::Core;

class ConcertRoad : public Component {
	public:
        static ConcertRoad* instance;
        std::unordered_set<Generation::MapHexTile*> RoadMapPoints;
        Prefab* Marker = nullptr;
        int NumberOfPoints = 3;

        bool ConsiderInfluenced = false;

        //TextMeshProUGUI textField;

        std::vector<int> entityPoints;
        std::vector<float> entityMultiplier;

        float maxMultiplier = 2.0f;
        float minMultiplier = 1.0f;
        glm::vec4 mulPercentage = glm::vec4(0.0f, 30.0f, 60.0f, 90.0f);
        float height = 1.0f;


        virtual void Initialize() override;
        virtual void Update() override;
        void Use();
        void Begin();
        void Finish();
};