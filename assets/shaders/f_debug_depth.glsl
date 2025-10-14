#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

// Para projeção ORTOGRÁFICA, não precisa linearizar!
// A profundidade já é linear
float LinearizeDepth(float depth)
{
    // Para ortho, depth já está em [0,1] linear
    return depth;
}

void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    
    // Para debug: apenas mostrar o valor direto
    // Se estiver tudo preto, o depth buffer está vazio
    // Se mostrar gradientes de cinza, está funcionando
    FragColor = vec4(vec3(depthValue), 1.0);
}
