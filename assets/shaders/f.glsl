#version 330

const int MAX_POINT_LIGHTS = 6;
const int MAX_SPOT_LIGHTS = 6;

uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    float shininess;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

struct BaseLight
{
	vec3 color;
	float ambient;
	float diffuse;
};

struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Exp;
};


struct PointLight
{
	BaseLight base;
    vec3 position;

    float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	PointLight pointLightBase;
	vec3 Direction;
	float cutoff;
    float outerCutOff;
    vec3 direction;
};


uniform int point_lights_size;
uniform PointLight point_lights[MAX_POINT_LIGHTS];

uniform int spot_lights_size;
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];



struct Light
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform DirectionalLight directional_light;

uniform Material material;

out vec4 fragColor;

in vec3 vNormal;
in vec3 fragPos;
in vec2 texCoords;

in vec3 normal;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 result = CalcDirLight(directional_light, normal, viewDirection);
    
    for(int i = 0; i < point_lights_size; i++)
    {
        result += CalcPointLight(point_lights[i], norm, fragPos, viewDirection);
    }

    for(int i = 0; i < spot_lights_size; i++)
    {   
        result += CalcSpotLight(spot_lights[i], norm, fragPos, viewDirection);
    }
    
    fragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)

{
    vec3 lightDirection = normalize(-directional_light.direction);
    // diffuse
    float diff = max(dot(normal, lightDirection), 0.0);
    
    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);

    vec3 ambient =  directional_light.base.ambient * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 diffuse = directional_light.base.diffuse * diff * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 specular = 1.0 * spec * texture(material.texture_specular1, texCoords).xyz;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);

    // attenuation
    float distance_ = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance_ + light.quadratic * (distance_ * distance_));

    vec3 ambient = light.base.color * directional_light.base.ambient * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 diffuse = light.base.color * directional_light.base.diffuse * diff * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 specular = light.base.color *   1.0 * spec * texture(material.texture_specular1, texCoords).xyz;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.pointLightBase.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

      // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);

      // attenuation
    float distance_ = length(light.pointLightBase.position - fragPos);
    float attenuation = 1.0 / (light.pointLightBase.constant + light.pointLightBase.linear * distance_ + light.pointLightBase.quadratic * (distance_ * distance_));

    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.pointLightBase.base.color * light.pointLightBase.base.ambient * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 diffuse = light.pointLightBase.base.color * light.pointLightBase.base.diffuse * diff * texture(material.texture_diffuse1, texCoords).xyz;
    vec3 specular = light.pointLightBase.base.color *   1.0 * spec * texture(material.texture_specular1, texCoords).xyz;


    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}