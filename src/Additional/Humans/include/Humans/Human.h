#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/Random.h>

#include <Humans/HumanMovement.h>

#include <Generation/CitiesManager.h>

namespace Humans
{
	class Human : public Twin2Engine::Core::Component
	{

        float achievingDestinationAccuracity = 0.2f;


        HumanMovement* _movement;

        Twin2Engine::Core::GameObject* _targetCity = nullptr;

    public:
        // Start is called before the first frame update
        void Initialize();

        // Update is called once per frame
        void Update();


        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
        virtual void DrawEditor() override;
	};
}