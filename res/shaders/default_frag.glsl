#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 w_Position;
in vec4 Normal;
in vec4 vertexColor;

void main()
{
    vec3 color;
    vec3 vColor = vec3(vertexColor);

    if (Normal.x > 0.9) { 
        color = vColor * 0.7;
    } 
    else if (Normal.x < -0.9) { 
        color = vColor * 0.45;
    } 
    else if (Normal.y > 0.9) { 
        color = vColor * 0.8;
    } 
    else if (Normal.y < -0.9) { 
        color = vColor * 0.35;
    } 
    else if (Normal.z > 0.9) { 
        color = vColor * 0.6;
    } 
    else if (Normal.z < -0.9) { 
        color = vColor * 0.3;
    } 
    else {
        color = vColor * 0.5;
    }

    FragColor = vec4(color, vertexColor.w);
}