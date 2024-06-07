#pragma once

#include <core/ScriptableObject.h>
#include <graphic/Material.h>

ENUM_CLASS_BASE_VALUE(TILE_COLOR, uint8_t, BLUE, 0, RED, 1, GREEN, 2, PURPLE, 4, YELLOW, 8, CYAN, 16, PINK, 32);

class HexTileTextureData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(HexTileTextureData)

private:
    //std::vector<std::vector<std::string>> _materialNames;

    //std::vector<std::vector<Twin2Engine::Graphic::Material*>> DeserializationHelperMaterialNamesToMaterials(std::vector<std::vector<std::string>> materialNames);
    //void DeserializationHelperMaterialNamesToMaterials(std::vector<std::vector<std::string>> materialNames);
    //void DeserializationHelperMaterialNamesToMaterials();

    //std::vector<std::vector<Twin2Engine::Graphic::Material*>> _materials;

    Twin2Engine::Graphic::Material* _neutralMaterial;
    //std::string _neutralMaterialName;

    /*
    std::vector<std::string> _blueMaterialsNames;
    std::vector<std::string> _redMaterialsNames;
    std::vector<std::string> _greenMaterialsNames;
    std::vector<std::string> _purpleMaterialsNames;
    std::vector<std::string> _yellowMaterialsNames;
    std::vector<std::string> _cyanMaterialsNames;
    std::vector<std::string> _pinkMaterialsNames;
    */

    std::vector<Twin2Engine::Graphic::Material*> _blueMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _redMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _greenMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _purpleMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _yellowMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _cyanMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _pinkMaterials;

public:

    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG

    virtual void DrawEditor() override
    {
        // Nothing Here
    }

#endif

    virtual void Clear() override {

        /*
        _blueMaterialsNames.clear();
        _redMaterialsNames.clear();
        _greenMaterialsNames.clear();
        _purpleMaterialsNames.clear();
        _yellowMaterialsNames.clear();
        _cyanMaterialsNames.clear();
        _pinkMaterialsNames.clear();
        */

        _neutralMaterial = nullptr;
        _blueMaterials.clear();
        _redMaterials.clear();
        _greenMaterials.clear();
        _purpleMaterials.clear();
        _yellowMaterials.clear();
        _cyanMaterials.clear();
        _pinkMaterials.clear();

        /*
        for (size_t i = 0; i < _materials.size(); ++i)
        {
            _materials[i].clear();
            _materialNames[i].clear();
        }

        _materials.clear();
        _materialNames.clear();
        */
    }

    Twin2Engine::Graphic::Material* GetMaterial(TILE_COLOR color, size_t stage);
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(HexTileTextureData);