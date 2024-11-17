#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 w_Position;
out vec4 Normal;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    w_Position = model * vec4(aPos, 1.0);

    Normal = vec4(aNormal, 0.0);
}
