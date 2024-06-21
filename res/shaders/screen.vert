#version 450 core
layout (location = 0) in vec3 position;
//layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

float map(float x, float currStart, float currEnd, float expectedStart, float expectedEnd) 
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (x - currStart);
}

void main()
{
    //gl_Position = vec4(map(position.x, -0.5, 0.5, -1, 1), map(position.z, -0.5, 0.5, 1, -1), 0.0, 1.0);
    //TexCoord = vec2(texCoord.x, map(texCoord.y, 0, 1, 1, 0));

    gl_Position = vec4(position.x, map(position.y, 1.0, -1.0, -1.0, 1.0), 0.0, 1.0);
    TexCoord = vec2(map(position.x, -1.0, 1.0, 0.0, 1.0), map(position.y, 1.0, -1.0, 0.0, 1.0));
}  