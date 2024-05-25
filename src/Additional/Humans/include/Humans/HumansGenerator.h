#pragma once

#include <core/GameObject.h>
#include <core/Prefab.h>
#include <core/Random.h>

#include <manager/SceneManager.h>

#include <Generation/CitiesManager.h>

#include <Generation/Generators/AMapElementGenerator.h>

#include <Humans/HumanMovement.h>

namespace Humans
{
	class HumansGenerator : public Generation::Generators::AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(HumansGenerator)
	private:

		Twin2Engine::Core::Prefab* prefabHuman;
		unsigned int number = 0;

	public:

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap);

		SO_SERIALIZE();
		SO_DESERIALIZE();

#if _DEBUG
		virtual void DrawEditor() override
		{
			// TODO: Dodac prefab Human do edytora
			ImGui::DragInt("Density", (int*)&number, 1, 0);
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(HumansGenerator, Humans)