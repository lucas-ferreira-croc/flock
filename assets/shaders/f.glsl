#version 330

const int MAX_POINT_LIGHTS = 6;
const int MAX_SPOT_LIGHTS = 6;

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
	vec3 local_pos;
	Attenuation attenuation;
};

struct SpotLight
{
	PointLight PointLightBase;
	vec3 Direction;
	float Cutoff;
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
    vec3 ambient =  directional_light.base.ambient * texture(material.texture_diffuse1, texCoords).xyz;
    
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(-directional_light.direction);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = directional_light.base.diffuse * diff * texture(material.texture_diffuse1, texCoords).xyz;
    
    float specularStrentgh = 0.5;
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = lightColor * spec * texture(material.texture_specular1, texCoords).xyz;
    vec3 result = (ambient + diffuse + specular);
    
    fragColor = vec4(result, 1.0);
}