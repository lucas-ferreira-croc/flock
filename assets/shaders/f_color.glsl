#version 330

const int MAX_POINT_LIGHTS = 6;
const int MAX_SPOT_LIGHTS = 6;

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
	vec3 direction;
	float cutoff;
    float outerCutOff;
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

uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

out vec4 fragColor;

in vec3 vNormal;
in vec3 fragPos;
in vec3 normal;
in vec4 fragPosLightSpace;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

uniform sampler2D shadowMap;

float shadowCalculations(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    float bias = 0.005;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}


void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 lighting  = CalcDirLight(directional_light, norm, viewDirection);

//     for(int i = 0; i < point_lights_size; i++)
//     {
//         lighting  += CalcPointLight(point_lights[i], norm, fragPos, viewDirection);
//     }

//    for(int i = 0; i < spot_lights_size; i++)
//    {   
//        lighting  += CalcSpotLight(spot_lights[i], norm, fragPos, viewDirection);
//    }

    fragColor = vec4(lighting , 1.0);
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //fragColor = vec4(vec3(depth), 1.0);
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDirection = normalize(-directional_light.direction);
    // diffuse
    float diff = max(dot(normal, lightDirection), 0.0);
    
    // specular
    //vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

    vec3 ambient = light.base.color * directional_light.base.ambient * material.ambient;
    vec3 diffuse = light.base.color * directional_light.base.diffuse * (diff * material.diffuse);
    vec3 specular = directional_light.base.color * (spec * material.specular);

    float shadow = shadowCalculations(fragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * light.base.color;
    //return (ambient + diffuse + specular);
    return lighting;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // specular
    //vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

    // attenuation
    float distance_ = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance_ + light.quadratic * (distance_ * distance_));

    vec3 ambient = light.base.color * light.base.ambient * material.ambient;
    vec3 diffuse = light.base.color * light.base.diffuse * (diff * material.diffuse);
    vec3 specular = light.base.color * (spec * material.specular);

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
    //vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 halfwayDirection = normalize(lightDirection + viewDir);

    float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

    // attenuation
    float distance_ = length(light.pointLightBase.position - fragPos);
    float attenuation = 1.0 / (light.pointLightBase.constant + light.pointLightBase.linear * distance_ + light.pointLightBase.quadratic * (distance_ * distance_));

    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.pointLightBase.base.color * light.pointLightBase.base.ambient * material.ambient;
    vec3 diffuse = light.pointLightBase.base.color * light.pointLightBase.base.diffuse * (diff * material.diffuse);
    vec3 specular = light.pointLightBase.base.color * (spec * material.specular);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation* intensity;
    return (ambient + diffuse + specular);
}