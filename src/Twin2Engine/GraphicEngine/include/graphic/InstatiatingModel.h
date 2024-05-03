#ifndef INSTANTIATING_MODEL
#define INSTANTIATING_MODEL

#include <graphic/InstatiatingMesh.h>
#include <graphic/ModelData.h>

namespace Twin2Engine
{
    namespace Manager {
        class ModelsManager;
    }

    namespace Graphic {
        class InstatiatingModel
        {
        private:

            friend class Manager::ModelsManager;

            ModelData* modelData;

            //void setupModel(const vector<glm::mat4>& transforms);

            InstatiatingModel(ModelData* modelData);

        public:
            InstatiatingModel(const InstatiatingModel&);
            InstatiatingModel(InstatiatingModel&&);
            InstatiatingModel();

            //InstatiatingModel(const char* path, const vector<glm::mat4>& transforms);
            ~InstatiatingModel();

            size_t GetId() const;
            size_t GetMeshCount() const;
            InstatiatingMesh* GetMesh(size_t index) const;

            //void Draw(Shader* shaderProgram);

            //void SetTransform(unsigned int meshIndex, const vector<Transform*> transforms);

            InstatiatingModel& operator=(const InstatiatingModel& other);
            InstatiatingModel& operator=(InstatiatingModel&& other);
            InstatiatingModel& operator=(std::nullptr_t);
            bool operator==(std::nullptr_t);
            bool operator!=(std::nullptr_t);
            bool operator==(const InstatiatingModel& other);
            bool operator!=(const InstatiatingModel& other);
        };
    }
}

#endif // !INSTANTIATING_MODEL
