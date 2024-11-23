#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 w_Position;  
in vec4 Normal;      
in vec4 vertexColor; 

uniform vec3 fogColor = vec3(0.0, 0.7, 1.0);
uniform float fogStart = 100;
uniform float fogEnd = 150;

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

    // Calculate fog factor based on depth
    float depth = gl_FragCoord.z / gl_FragCoord.w; 
    float fogFactor = clamp((fogEnd - depth) / (fogEnd - fogStart), 0.0, 1.0);

    vec3 finalColor = mix(fogColor, color, fogFactor);

    // Output the final color
    FragColor = vec4(finalColor, vertexColor.w);
}
