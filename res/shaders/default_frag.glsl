#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 w_Position;
in vec4 Normal;

void main()
{
    vec3 color;

    // Assign different shades of green based on the normal direction
    if (Normal.x > 0.9) { 
        color = vec3(0.0, 0.8, 0.0);
    } 
    else if (Normal.x < -0.9) { 
        color = vec3(0.0, 0.5, 0.0); 
    } 
    else if (Normal.y > 0.9) { 
        color = vec3(0.0, 0.9, 0.0);
    } 
    else if (Normal.y < -0.9) { 
        color = vec3(0.0, 0.3, 0.0); 
    } 
    else if (Normal.z > 0.9) { 
        color = vec3(0.0, 0.6, 0.0); 
    } 
    else if (Normal.z < -0.9) { 
        color = vec3(0.0, 0.4, 0.0); 
    } 
    else {
        color = vec3(0.0, 0.5, 0.0); 
    }

    FragColor = vec4(color, 1.0);
}