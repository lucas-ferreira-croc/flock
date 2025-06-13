#version 330

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 textureCoordinates;

out vec3 posColor;
out vec2 texCoords;

uniform mat4 wvp;

void main()
{
    gl_Position = wvp* vec4(position, 1.0);
    posColor = vertColor;
    texCoords = textureCoordinates;
}