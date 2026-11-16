#version 330 core

in vec3 direction;
out vec4 color;
uniform samplerCube sky;

void main() {
    color = vec4(texture(sky, direction).rgb, 1.0);
}
