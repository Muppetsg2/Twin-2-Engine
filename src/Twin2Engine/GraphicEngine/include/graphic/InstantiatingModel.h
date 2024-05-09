#pragma once

//#include <graphic/Model.h>
#include <graphic/InstantiatingMesh.h>
#include <graphic/ModelData.h>

namespace Twin2Engine
{
    namespace Manager {
        class ModelsManager;
    }

    namespace Graphic {
        class InstantiatingModel
        {
        private:

            friend class Manager::ModelsManager;

            ModelData* modelData;

            //void setupModel(const vector<glm::mat4>& transforms);

            InstantiatingModel(ModelData* modelData);

        public:
            InstantiatingModel(const InstantiatingModel&);
            InstantiatingModel(InstantiatingModel&&);
            InstantiatingModel();

            //InstantiatingModel(const char* path, const vector<glm::mat4>& transforms);
            ~InstantiatingModel();

            size_t GetId() const;
            size_t GetMeshCount() const;
            InstantiatingMesh* GetMesh(size_t index) const;

            //void Draw(Shader* shaderProgram);

            //void SetTransform(unsigned int meshIndex, const vector<Transform*> transforms);

            InstantiatingModel& operator=(const InstantiatingModel& other);
            InstantiatingModel& operator=(InstantiatingModel&& other);
            InstantiatingModel& operator=(std::nullptr_t);
            bool operator==(std::nullptr_t);
            bool operator!=(std::nullptr_t);
            bool operator==(const InstantiatingModel& other);
            bool operator!=(const InstantiatingModel& other);
        };
    }
}