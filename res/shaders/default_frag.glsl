#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 w_Position;
in vec4 Normal;
in vec3 vertexColor;

void main()
{
    vec3 color;

    if (Normal.x > 0.9) { 
        color = vertexColor * 0.7;
    } 
    else if (Normal.x < -0.9) { 
        color = vertexColor * 0.45;
    } 
    else if (Normal.y > 0.9) { 
        color = vertexColor * 0.8;
    } 
    else if (Normal.y < -0.9) { 
        color = vertexColor * 0.35;
    } 
    else if (Normal.z > 0.9) { 
        color = vertexColor * 0.6;
    } 
    else if (Normal.z < -0.9) { 
        color = vertexColor * 0.3;
    } 
    else {
        color = vertexColor * 0.5;
    }

    FragColor = vec4(color, 1.0);
}