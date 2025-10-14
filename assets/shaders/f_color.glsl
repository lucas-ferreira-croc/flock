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
float far = 15.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

uniform sampler2D shadowMap;

// float shadowCalculations(vec4 fragPosLightSpace, vec3 fragNormal)
// {
//     // Perform perspective divide
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
//     // Transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;
    
//     // Check if outside light frustum
//     if(projCoords.z > 1.0)
//         return 0.0;
    
//     if(projCoords.x < 0.0 || projCoords.x > 1.0 || 
//        projCoords.y < 0.0 || projCoords.y > 1.0)
//         return 0.0;
    
//     // Get closest depth value from light's perspective
//     float closestDepth = texture(shadowMap, projCoords.xy).r;
    
//     // Get depth of current fragment from light's perspective  
//     float currentDepth = projCoords.z;
    
//     // Calculate bias (based on surface angle toward light)
//     vec3 norm = normalize(fragNormal);
//     vec3 lightDir = normalize(-directional_light.direction);
    
//     // Bias constante pequeno primeiro - depois ajustar dinamicamente se necessário
//      float bias = 0.0001;
    
    
//     // PCF (Percentage Closer Filtering)
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//     for(int x = -1; x <= 1; ++x)
//     {
//         for(int y = -1; y <= 1; ++y)
//         {
//             float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
//         }    
//     }
//     shadow /= 9.0;
    
//     return shadow;
// }
float shadowCalculations(vec4 fragPosLightSpace, vec3 lightDir, vec3 norm)
{
 // Projeta para o espaço de recorte do light view
     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;  // <- mantém linearizado [0,1]
    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.0005);

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}  


void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPos - fragPos);
   // vec3 lighting  = CalcDirLight(directional_light, norm, viewDirection);

    vec3 color = material.diffuse;
    vec3 lightColor = vec3(0.7);
    vec3 ambient = 0.3 * lightColor;
    vec3 lightDir = normalize(vec3(0, 4, 1) - fragPos);

     float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = shadowCalculations(fragPosLightSpace, lightDir, norm);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    // Converte para [0, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Converte para [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    //fragColor = vec4(texture(shadowMap, projCoords.xy).rrr, 1.0);

    fragColor = vec4(lighting, 1.0);
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

    //float shadow = shadowCalculations(fragPosLightSpace, lightDirection);
    //vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    return ambient + diffuse + specular;
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