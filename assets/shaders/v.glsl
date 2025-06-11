#version 330

layout(location = 0) in vec3 position; 

out vec3 posColor;

void main()
{
    gl_Position = vec4(position, 1.0);
    posColor = position;
}