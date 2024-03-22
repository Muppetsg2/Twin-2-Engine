#ifndef INSTANTIATING_MODEL
#define INSTANTIATING_MODEL

#include "Model.h"
#include "InstatiatingMesh.h"
#include "Transform.h"

class InstatiatingModel
{
    Model* model = nullptr;
    vector<InstatiatingMesh> meshes;

    void setupModel(const vector<glm::mat4>& transforms)
    {
        for (Mesh& mesh : model->meshes)
        {
            meshes.push_back(InstatiatingMesh(&mesh, transforms));
        }
    }

public:

    InstatiatingModel(const char* path, const vector<glm::mat4>& transforms)
    {
        model = new Model(path);

        setupModel(transforms);
    }
    ~InstatiatingModel()
    {
        delete model;
    }

    void Draw(Shader* shaderProgram)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shaderProgram);
    }

    void SetTransform(unsigned int meshIndex, const vector<Transform*> transforms)
    {
        vector<glm::mat4> transformsMatrixes;
        for (Transform* transform : transforms)
        {
            transform->RecalculateTransform();
            if (transform->GerHierachyActive())
            {
                transformsMatrixes.push_back(transform->transform);
            }
        }

        meshes[meshIndex].SetNewTransforms(transformsMatrixes);
    }
};

#endif // !INSTANTIATING_MODEL
