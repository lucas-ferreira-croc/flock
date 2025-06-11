#version 330

out vec4 FragColor;

in vec3 posColor;

void main()
{
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = vec4(posColor, 1.0);
}