#version 330

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 textureCoordinates;

out vec3 posColor;
out vec2 texCoords;

void main()
{
    gl_Position = vec4(position, 1.0);
    posColor = position;
    texCoords = textureCoordinates;
}