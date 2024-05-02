#ifndef _PROCESSING_MTL_FILES_H_
#define _PROCESSING_MTL_FILES_H_

#include <core/YamlConverters.h>

namespace Editor::Common
{

    void processMTLFiles(const std::filesystem::path& inputDir, const std::filesystem::path& outputDir);

    void parseMaterial(const std::filesystem::path& filePath, const std::filesystem::path& dstDirectory);

    void saveMaterialFromMTLFiles(const aiMaterial* material, const std::filesystem::path& dstDirectory, const std::filesystem::path& srcModel);

    void saveTextures(const aiMaterial* material, aiTextureType textureType, YAML::Emitter& emitter, const std::string& prefix);
}


#endif // !_PROCESSING_MTL_FILES_H_