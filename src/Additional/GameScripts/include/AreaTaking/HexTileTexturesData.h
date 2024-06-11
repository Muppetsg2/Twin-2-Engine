#pragma once

#include <core/ScriptableObject.h>
#include <graphic/Material.h>

ENUM_CLASS_BASE_VALUE(TILE_COLOR, uint8_t, NEUTRAL, 0, BLUE, 1, RED, 2, GREEN, 4, PURPLE, 8, YELLOW, 16, CYAN, 32, PINK, 64);

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
    Twin2Engine::Graphic::Material* _neutralBorderMaterial;
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
    std::vector<Twin2Engine::Graphic::Material*> _blueRetreatingMaterials;
    Twin2Engine::Graphic::Material* _blueBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _redMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _redRetreatingMaterials;
    Twin2Engine::Graphic::Material* _redBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _greenMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _greenRetreatingMaterials;
    Twin2Engine::Graphic::Material* _greenBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _purpleMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _purpleRetreatingMaterials;
    Twin2Engine::Graphic::Material* _purpleBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _yellowMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _yellowRetreatingMaterials;
    Twin2Engine::Graphic::Material* _yellowBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _cyanMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _cyanRetreatingMaterials;
    Twin2Engine::Graphic::Material* _cyanBorderMaterial;

    std::vector<Twin2Engine::Graphic::Material*> _pinkMaterials;
    std::vector<Twin2Engine::Graphic::Material*> _pinkRetreatingMaterials;
    Twin2Engine::Graphic::Material* _pinkBorderMaterial;

public:

    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG

    virtual void DrawEditor() override
    {
        // Nothing Here
    }

#endif

    virtual void Clear() override;

    Twin2Engine::Graphic::Material* GetMaterial(TILE_COLOR color, size_t stage);
    Twin2Engine::Graphic::Material* GetRetreatingMaterial(TILE_COLOR color, size_t stage);
    Twin2Engine::Graphic::Material* GetBorderMaterial(TILE_COLOR color);
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(HexTileTextureData);