#version 330

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

uniform Material material;

out vec4 fragColor;

in vec3 vNormal;
in vec3 fragPos;
in vec2 texCoords;

in vec3 normal;

void main()
{
    vec3 ambient =  light.ambient * material.ambient;
    
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    float specularStrentgh = 0.5;
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);

    // float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 256);
    // vec3 specular = specularStrentgh * spec * lightColor;
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);
    vec3 result = (ambient + diffuse + specular) *  vec3(texture(material.texture_diffuse1, texCoords).xyz);
    
    fragColor = vec4(result, 1.0);
}