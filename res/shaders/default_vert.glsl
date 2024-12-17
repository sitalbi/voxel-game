#version 330 core
layout(location = 0) in vec3 aPos;       
layout(location = 1) in vec3 aNormal;    
layout(location = 2) in vec4 aColor;     

uniform mat4 model;            
uniform mat4 view;             
uniform mat4 projection;       
uniform mat4 lightSpaceMatrix; 
uniform vec3 lightDir;
uniform vec3 ambientLight = vec3(0.25, 0.25, 0.25); 
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

out vec4 vertexColor;      
out vec4 fragPosLightSpace;
out vec3 lightDirection;   
out vec4 normal;

void main()
{
    // Transform the vertex position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    normal = model * vec4(aNormal, 0.0);

    // Compute light
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    float diff = max(dot(normal.xyz, normalize(lightDir)), 0.0);
    vec3 lighting = (ambientLight + diff) * lightColor;

    // Pass to the fragment shader
    vertexColor = aColor * vec4(lighting, 1.0);
    fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    lightDirection = normalize(lightDir);
}
