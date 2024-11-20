#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube u_skybox;

void main() {
    vec4 color = texture(u_skybox, TexCoords);
    FragColor = vec4(color.rgb, 1.0);
}
