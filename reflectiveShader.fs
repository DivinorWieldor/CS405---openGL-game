#version 330 core
#define NR_POINT_LIGHTS 1
struct Material {
    sampler2D specular;//reflection intensity + texture Of Object
    float shininess;//metalicity
    sampler2D  diffuse;//color + texture Of Object
    sampler2D  emission;//color + texture Of Object
}; 

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
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main(){
  	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);//directional light
    for(int i = 0; i < NR_POINT_LIGHTS; i++)// phase 2: point lights
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);// phase 3: spot light

            
    FragColor = vec4(result, 1.0);
}

//----------------------------------------------------------- Lighting Functions ----------------------------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){//directional light
    vec3 lightDir = normalize(-light.direction);

    return (light.ambient * vec3(texture(material.diffuse, TexCoords)) +
            light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords)) +
            light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords)));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){//point light
    vec3 lightDir = normalize(light.position - fragPos);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));// attenuation

    return (light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuation +
            light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords)) * attenuation +
            light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords)) * attenuation);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){//spot light
    vec3 lightDir = normalize(light.position - fragPos);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));// attenuation
    
    //                                                theta-------------------------------------                        epsilon-------------------------
    float attenuationIntensity = attenuation * clamp((dot(lightDir, normalize(-light.direction)) - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);// spotlight intensity

    return (light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuationIntensity +
            light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords)) * attenuationIntensity +
            light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords)) * attenuationIntensity);
}