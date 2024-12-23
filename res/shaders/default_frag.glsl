#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 normal;          
in vec4 vertexColor;     
in vec4 fragPosLightSpace; 
in vec3 lightDirection;

uniform sampler2D shadowMap;
uniform vec3 fogColor = vec3(0.0, 0.7, 1.0);
uniform float fogStart = 100;
uniform float fogEnd = 150;
uniform bool useShadows = true;
uniform bool isDay;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Transform to normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Check if fragment is outside the shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.1;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float bias = max(0.0075 * (1.0 - dot(normal.xyz, lightDirection)), 0.0001);
    //float bias = 0.00005;
//    float diffuseFactor = max(dot(normal.xyz, -lightDirection),0.0);
//    float bias = mix(0.0,0.0,diffuseFactor);


    // Percentage-Closer Filtering
//     float shadow = 0.0;
//     int samples = 2; 
//     float texelSize = 1.0 / textureSize(shadowMap, 0).x;
//    
//     for (int x = -samples; x <= samples; x++)
//     {
//         for (int y = -samples; y <= samples; y++)
//         {
//             vec2 offset = vec2(x, y) * texelSize;
//             float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
//             shadow += currentDepth > closestDepth + bias ? 0.75 : 0.1;
//         }
//     }
//     shadow /= pow((samples * 2 + 1), 2);

    // No PCF
    float shadow = currentDepth > closestDepth ? 0.75 : 0.1;

    return shadow;
}

void main()
{
    vec3 finalColor;
    if(useShadows)
    {
        if(!isDay) {
            finalColor = 0.75 * vertexColor.rgb;
        } else {
            // tmp fix for shadow acne on vertical faces
            if(dot(normal.xyz, lightDirection) > 0.01) {
                float shadow = ShadowCalculation(fragPosLightSpace);
                finalColor = (1.0 - shadow) * vertexColor.rgb;
            } else {
                finalColor = vertexColor.rgb;
            }
            
        }
    }
    else
    {
        finalColor = vertexColor.rgb * 0.9;
    }

//    float depth = gl_FragCoord.z / gl_FragCoord.w; 
//    float fogFactor = clamp((fogEnd - depth) / (fogEnd - fogStart), 0.0, 1.0);
//
//    finalColor = mix(fogColor, finalColor, fogFactor);

    FragColor = vec4(finalColor, vertexColor.w);
}
