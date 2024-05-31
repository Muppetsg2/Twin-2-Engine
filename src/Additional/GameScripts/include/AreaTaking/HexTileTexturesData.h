#pragma once

#include <core/ScriptableObject.h>
#include <graphic/Material.h>
#include <graphic/manager/MaterialsManager.h>

class HexTileTextureData : public Twin2Engine::Core::ScriptableObject
{
    SCRIPTABLE_OBJECT_BODY(HexTileTextureData)

   std::vector<std::vector<std::string>> _materialNames;

   std::vector<std::vector<Twin2Engine::Graphic::Material>> DeserializationHelperMaterialNamesToMaterials(std::vector<std::vector<std::string>> materialNames);

public:
    std::vector<std::vector<Twin2Engine::Graphic::Material>> _materials;


    SO_SERIALIZE()
    SO_DESERIALIZE()

#if _DEBUG

    virtual void DrawEditor() override
    {
        //string id = to_string(GetId());
        //
        //ImGui::Text("Name: ");
        //ImGui::SameLine();
        //ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        //ImGui::Text(Twin2Engine::Manager::ScriptableObjectManager::GetName(GetId()).c_str());
        //ImGui::PopFont();
        //
        //
        //
        //ScriptableObject::DrawInheritedFields();
    }

#endif
};

SERIALIZABLE_SCRIPTABLE_OBJECT_NN(HexTileTextureData);