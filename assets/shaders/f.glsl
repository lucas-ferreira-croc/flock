#version 330

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

out vec4 fragColor;

in vec3 vNormal;
in vec3 fragPos;
in vec2 texCoords;

in vec3 normal;

void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);

    vec3 lightDirection = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrentgh = 0.5;
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 256);
    vec3 specular = specularStrentgh * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) *  vec3(texture(material.texture_diffuse1, texCoords).xyz);
    
    fragColor = vec4(result, 1.0);
}