#version 330 core

in vec3 worldPosition;
in vec3 normal;
in vec2 uv;
out vec4 color;

uniform sampler2D dayTexture, nightTexture;
uniform vec3 cameraPosition, tint;
uniform bool earth, nightBlend, emissive, headlight, textured, ring;
uniform float lightPower;

vec3 lightTerm(vec3 N, vec3 L, vec3 V, vec3 albedo, vec3 radiance) {
    float diffuse = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float specular = diffuse > 0.0 ? pow(max(dot(N, H), 0.0), 48.0) : 0.0;
    return radiance * (albedo * diffuse + vec3(specular * 0.20));
}

void main() {
    vec3 N = normalize(normal);
    vec3 V = normalize(cameraPosition - worldPosition);
    vec3 albedo = textured ? texture(dayTexture, uv).rgb : tint;

    if (emissive) {
        color = vec4(pow(albedo * 1.5, vec3(1.0 / 2.2)), 1.0);
        return;
    }

    // The Sun is the primary point light at the scene origin.
    vec3 L = normalize(-worldPosition);
    float nDotL = dot(N, L);
    vec3 result = albedo * 0.035;
    result += lightTerm(N, L, V, albedo, vec3(1.25, 1.16, 1.02));

    if (earth && nightBlend) {
        float night = 1.0 - smoothstep(-0.12, 0.16, nDotL);
        result += texture(nightTexture, uv).rgb * night * 2.3;
    }

    if (headlight) {
        float distance = length(cameraPosition - worldPosition);
        vec3 radiance = vec3(0.65, 0.78, 1.0) * lightPower;
        radiance /= 1.0 + 0.035 * distance * distance;
        result += lightTerm(N, V, V, albedo, radiance);
    }

    float alpha = 1.0;
    if (ring) {
        float bands = 0.54 + 0.22 * sin(uv.x * 170.0) + 0.12 * sin(uv.x * 47.0);
        float gap = 1.0 - 0.93 * exp(-pow((uv.x - 0.48) / 0.025, 2.0));
        alpha = clamp(bands * gap, 0.04, 0.82);
        result = tint * (0.28 + 0.65 * abs(dot(N, L)));
    }
    color = vec4(pow(max(result, vec3(0.0)), vec3(1.0 / 2.2)), alpha);
}
