#version 330

out vec4 fragColor;

in vec3 posColor;
in vec2 texCoords;

void main()
{
    //fragColor = vec4(1.0, 0.0, 1.0, 1.0);
    //fragColor = vec4(posColor, 1.0);
    //texture(textureSampler, texCoords);
    //fragColor = texture(material.texture_diffuse1, texCoords);
    fragColor = vec4(0.5804, 0.051, 0.9098, 1.0);
}