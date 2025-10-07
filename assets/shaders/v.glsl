#version 330

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 textureCoordinates;

out vec2 texCoords;
out vec3 vNormal;
out vec3 fragPos;
out vec3 normal;
out vec4 fragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vNormal = vertNormal;
    normal = mat3(transpose(inverse(model))) * vertNormal;

    texCoords = textureCoordinates;
    fragPos = vec3(model * vec4(position, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}