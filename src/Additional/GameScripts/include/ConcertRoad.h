#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <unordered_set>
#include <vector>
#include <Generation/MapHexTile.h>
#include <AreaTaking/HexTile.h>

#include <processes/Coroutine.h>
//#include <pch.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Processes;

class ConcertRoad : public Component {
        Prefab* _marker = nullptr;
        std::vector<Playable*> _playables;

        float maxMultiplier = 2.0f;
        float minMultiplier = 1.0f;
        float maxBonus = 20.0f;
        float minBonus = 5.0f;
        glm::vec4 bonusesPerStage = glm::vec4(0.0f, 5.0f, 12.0f, 20.0f);
        float height = 0.0f;
        //Coroutine* countDownCoroutine = nullptr;
        //int maxValueOfTime = 30;
        //int curValueOfTime = 30;

	public:
        bool isPerforming = false;
        std::vector<GameObject*> concertRoadMarkers;
        float bonusDecreseCoef = 1.0f;
        static ConcertRoad* instance;

        //struct ConcertRoadPoint
        //{
        //    HexTile* hexTile;
        //    Playable* owningPlayable;
        //    float addedBonus;
        //    float possibleBonus; // wa¿ne tylko z punktu widzenia Playera
        //};
        //std::vector<ConcertRoadPoint> RoadMapPoints;
        std::vector<HexTile*> RoadMapPoints;
        //std::vector<HexTile*> RoadMapPoints;
        int NumberOfPoints = 3;

        bool ConsiderInfluenced = false;

        //TextMeshProUGUI textField;


        //std::vector<int> entityPoints;
        //std::vector<float> entityMultiplier;



        virtual void Initialize() override;
        virtual void Update() override;
        virtual void OnDestroy() override;
        void Use();
        void Begin();
        void Finish();

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
        virtual void DrawEditor() override;
#endif
};