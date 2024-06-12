#version 450 core
layout (location = 0) in vec2 position;

out vec2 TexCoord;

float map(float x, float currStart, float currEnd, float expectedStart, float expectedEnd) 
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (x - currStart);
}

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoord = vec2(map(position.x, -1.0, 1.0, 0.0, 1.0), map(position.y, 1.0, -1.0, 0.0, 1.0));
}  