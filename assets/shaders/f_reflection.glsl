#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    vec3 eye = normalize(fragPos - cameraPos);
    vec3 reflection = reflect(eye, normalize(normal));
    fragColor = vec4(texture(skybox, reflection).rgb, 1.0);
}