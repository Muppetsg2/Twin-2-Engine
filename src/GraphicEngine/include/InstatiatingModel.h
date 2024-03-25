#ifndef INSTANTIATING_MODEL
#define INSTANTIATING_MODEL

#include <Model.h>
#include <ModelData.h>
#include <InstatiatingMesh.h>
#include <glad/glad.h>


#include "../../Twin2Engine/inc/Transform.h"
//#include <Transform.h>

using Twin2EngineCore::Transform;

namespace GraphicEngine
{
    class Model;

    class InstatiatingModel
    {
    private:

        friend class ModelsManager;



        //Model* model = nullptr;
        //vector<InstatiatingMesh> meshes;

        ModelData* modelData;

        void setupModel(const vector<glm::mat4>& transforms);

        //static InstatiatingModel CreateModel(ModelData* modelData);
        InstatiatingModel(ModelData* modelData);

    public:
        InstatiatingModel(const InstatiatingModel&);
        InstatiatingModel(InstatiatingModel&&);
        InstatiatingModel();

        //InstatiatingModel(const char* path, const vector<glm::mat4>& transforms);
        ~InstatiatingModel();

        size_t GetMeshCount() const;
        InstatiatingMesh* GetMesh(size_t index) const;

        void Draw(Shader* shaderProgram);

        void SetTransform(unsigned int meshIndex, const vector<Transform*> transforms);

        InstatiatingModel& operator=(const InstatiatingModel& other);
        InstatiatingModel& operator=(InstatiatingModel&& other);
        InstatiatingModel& operator=(std::nullptr_t);
        bool operator==(std::nullptr_t);
        bool operator!=(std::nullptr_t);
        bool operator==(const InstatiatingModel& other);
        bool operator!=(const InstatiatingModel& other);
    };
}

//#include <ModelsManager.h>

#endif // !INSTANTIATING_MODEL
