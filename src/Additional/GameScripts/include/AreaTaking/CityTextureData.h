#pragma once

#include <core/ScriptableObject.h>
#include <graphic/Material.h>

#include <AreaTaking/HexTileTextureData.h>

class CityTextureData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(CityTextureData)

private:
    Twin2Engine::Graphic::Material* _neutralMaterial;

    Twin2Engine::Graphic::Material* _blueMaterial;

    Twin2Engine::Graphic::Material* _redMaterial;

    Twin2Engine::Graphic::Material* _greenMaterial;

    Twin2Engine::Graphic::Material* _purpleMaterial;

    Twin2Engine::Graphic::Material* _yellowMaterial;

    Twin2Engine::Graphic::Material* _cyanMaterial;

    Twin2Engine::Graphic::Material* _pinkMaterial;

public:

    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG

        virtual void DrawEditor() override
        {
            // TODO: Textury
        }

#endif

    virtual void Clear() override;

    virtual ~CityTextureData() = default;

    Twin2Engine::Graphic::Material* GetMaterial(TILE_COLOR color);
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(CityTextureData);