#version 330 core

layout(location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec4 FragPosLightSpace;

void main()
{
    FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
    
    gl_Position = FragPosLightSpace;
}
