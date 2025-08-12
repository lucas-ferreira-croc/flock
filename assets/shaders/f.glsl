#version 330

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

out vec4 fragColor;

in vec3 posColor;
in vec2 texCoords;

void main()
{
    //fragColor = vec4(1.0, 0.0, 1.0, 1.0);
    //fragColor = vec4(posColor, 1.0);
    //texture(textureSampler, texCoords);
    fragColor = texture(material.texture_diffuse1, texCoords);
    //fragColor = texture(textureSampler, texCoords) * vec4(0.0, 0.5, 0.5, 1.0);
}