#version 330 core
#define NR_POINT_LIGHTS 4

out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional Lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // Point Lights
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // Spot Light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // result
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    // results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
