#version 330 core

/*
Air	1.00
Water	1.33
Ice	1.309
Glass	1.52
Diamond	2.42
*/

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    float ratio = 1.00 / 1.52;
    vec3 eye = normalize(fragPos - cameraPos);
    vec3 refraction = refract(eye, normalize(normal), ratio);
    fragColor = vec4(texture(skybox, refraction).rgb, 1.0);
}