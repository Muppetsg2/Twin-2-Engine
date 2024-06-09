#pragma once

#include <core/GameObject.h>
#include <core/Prefab.h>
#include <core/Random.h>

#include <manager/SceneManager.h>

#include <Generation/CitiesManager.h>

#include <Generation/Generators/AMapElementGenerator.h>

#include <Humans/HumanMovement.h>
#include <Humans/Human.h>

namespace Humans
{
	class HumansGenerator : public Generation::Generators::AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(HumansGenerator)
	private:

		Twin2Engine::Core::Prefab* prefabHuman;
		unsigned int number = 0;

	public:

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		virtual void Clear() override;

		SO_SERIALIZE();
		SO_DESERIALIZE();

#if _DEBUG
		virtual void DrawEditor() override
		{
			// TODO: Dodac prefab Human do edytora
			ImGui::DragUInt(std::string("Density##").append(to_string(this->GetId())).c_str(), &number, 1, 0, UINT_MAX);
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(HumansGenerator, Humans)