#version 450 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    vec4 color = texture(particleTexture, texCoords);
    if (color.w == 0.0) {
        discard;
    }
    FragColor = color;
}