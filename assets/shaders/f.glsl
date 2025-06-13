#version 330

uniform sampler2D textureSampler;

out vec4 fragColor;

in vec3 posColor;
in vec2 texCoords;

void main()
{
    //fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    //fragColor = vec4(posColor, 1.0);
    //fragColor = texture(textureSampler, texCoords);
    fragColor = vec4(posColor, 1.0) * texture(textureSampler, texCoords);
    //fragColor = texture(textureSampler, texCoords) * vec4(0.0, 0.5, 0.5, 1.0);
}