#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 w_Position;
in vec4 Normal;
in vec2 TexCoords;

uniform sampler2D u_Texture;

void main()
{
    FragColor = texture(u_Texture, TexCoords);
}