#pragma once

#include <core/ScriptableObject.h>
#include <graphic/Material.h>
#include <graphic/manager/MaterialsManager.h>

class HexTileTextureData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(HexTileTextureData)

   std::vector<std::vector<std::string>> _materialNames;

   std::vector<std::vector<Twin2Engine::Graphic::Material*>> DeserializationHelperMaterialNamesToMaterials(std::vector<std::vector<std::string>> materialNames);

public:
    std::vector<std::vector<Twin2Engine::Graphic::Material*>> _materials;

    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG

    virtual void DrawEditor() override
    {
        // Nothing Here
    }

#endif

    virtual void Clear() override {
        for (size_t i = 0; i < _materials.size(); ++i) {
            _materials[i].clear();
            _materialNames[i].clear();
        }
        _materials.clear();
        _materialNames.clear();
    }
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(HexTileTextureData);