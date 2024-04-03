#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <glm/glm.hpp>

namespace Twin2Engine::GraphicEngine
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
}

#endif // !_VERTEX_H_

