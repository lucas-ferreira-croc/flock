#version 330

out vec4 fragColor;

void main()
{
    bool isColliding = false;
    vec3 color = isColliding ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}