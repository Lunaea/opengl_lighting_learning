#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    
    float cutOff;
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

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{   
    vec3 lightDir = normalize(light.position - FragPos);

    // spotlight
    float theta = dot(lightDir, normalize(-light.direction));

    if (theta > light.cutOff)
    {
        // diffuse
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        // ambient
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

        // emission
        vec3 emission = vec3(texture(material.emission, TexCoords));

        // attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // ambient *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due to ambient term in else branch
        diffuse *= attenuation;
        specular *= attenuation;



        vec3 result = ambient + diffuse + specular; // + emission;
        FragColor = vec4(result, 1.0f);
    }
    else
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
}
