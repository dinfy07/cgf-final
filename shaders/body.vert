#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aInstance;

uniform mat4 model, view, projection;
uniform bool instanced;
out vec3 worldPosition;
out vec3 normal;
out vec2 uv;

void main() {
    mat4 transform = model;
    if (instanced)
        transform = model * aInstance;
    vec4 position = transform * vec4(aPosition, 1.0);
    worldPosition = position.xyz;
    normal = mat3(transpose(inverse(transform))) * aNormal;
    uv = aUV;
    gl_Position = projection * view * position;
}
