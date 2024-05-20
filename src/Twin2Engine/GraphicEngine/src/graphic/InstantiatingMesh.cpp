#include <graphic/InstantiatingMesh.h>

using namespace Twin2Engine::Graphic;

InstantiatingMesh::InstantiatingMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    _vertices = std::vector<Vertex>();
    _vertices.swap(vertices);
    _indices = std::vector<unsigned int>();
    _indices.swap(indices);

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

    // pozycje wierzcho³ków
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // wektory normalne wierzcho³ków
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // wspó³rzedne tekstury wierzcho³ków
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);

#ifdef MESH_FRUSTUM_CULLING
    if (!_vertices.empty())
    {
        glm::vec3 p0 = _vertices[0].Position;

        // Step 2: Find the farthest point from X
        glm::vec3 p1 = p0;
        float maxDistSqr = 0;
        glm::vec3 t;
        float d;
        for (const auto& v : _vertices) {
            t = v.Position - p0;
            d = glm::dot(t, t);
            if (d > maxDistSqr) {
                maxDistSqr = d;
                p1 = v.Position;
            }
        }

        // Step 3: Find the farthest point from Y
        glm::vec3 p2 = p1;
        maxDistSqr = 0;
        for (const auto& v : _vertices) {
            t = v.Position - p1;
            d = glm::dot(t, t);
            if (d > maxDistSqr) {
                maxDistSqr = d;
                p2 = v.Position;
            }
        }

        // Initial sphere: center is the midpoint of Y and Z, radius is half the distance
        glm::vec3 center = (p1 + p2) * 0.5f;
        t = p2 - p1;
        maxDistSqr = glm::dot(t, t);
        float radius = glm::sqrt(maxDistSqr) * 0.5f;

        // Step 4: Expand sphere to include all points
        {
            float newRadius;
            float radiusDiff;
            glm::vec3 dir;
            for (const auto& v : _vertices) {
                t = v.Position - center;
                d = glm::dot(t, t);
                if (d > maxDistSqr) { // Point is outside the sphere
                    newRadius = (radius + glm::sqrt(d)) * 0.5f;
                    radiusDiff = newRadius - radius;
                    dir = glm::normalize(v.Position - center);
                    center = center - dir * radiusDiff;
                    radius = newRadius;
                    maxDistSqr = radius * radius;
                }
            }
        }

        //SPDLOG_INFO("BV has been created! R: {}", radius);
        sphericalBV = new Physic::BoundingVolume(new Physic::SphereColliderData);
        ((Physic::SphereColliderData*)sphericalBV->shapeColliderData)->LocalPosition = center;
        ((Physic::SphereColliderData*)sphericalBV->shapeColliderData)->Radius = radius;
    }
#endif // MESH_FRUSTUM_CULLING
}


InstantiatingMesh::~InstantiatingMesh()
{
    _vertices.clear();
    _indices.clear();

    glDeleteBuffers(1, &_EBO);
    glDeleteBuffers(1, &_VBO);
    glDeleteVertexArrays(1, &_VAO);

    #ifdef MESH_FRUSTUM_CULLING
    if (sphericalBV != nullptr) {
        delete sphericalBV;
    }
    #endif // MESH_FRUSTUM_CULLING
}

#ifdef MESH_FRUSTUM_CULLING
bool InstantiatingMesh::IsOnFrustum(Frustum& frustum, const glm::mat4& model)
{
    if (sphericalBV != nullptr) {
        sphericalBV->shapeColliderData->Position = glm::vec3(model * glm::vec4(sphericalBV->shapeColliderData->LocalPosition, 1.0f));
        return sphericalBV->isOnFrustum(frustum);
    }
    return true;
}
#endif // MESH_FRUSTUM_CULLING

void InstantiatingMesh::Draw(unsigned int number)
{
    glBindVertexArray(_VAO);

    glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, number);

    glBindVertexArray(0);
}