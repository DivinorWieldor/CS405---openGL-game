#version 330 core
#define NR_POINT_LIGHTS 4
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

struct Light {
    //position of the light
    vec3 position;//used for point light/spotlight
    //vec3 direction;//used for directional lights(sun in the sky etc)

    vec3 ambient;//generally set to a low value
    vec3 diffuse;//exact color we want light to have (often bright white)
    vec3 specular;//usually kept at 1 -> shining at full intensity

    //attenuation (decrease in intensity)
    float constant;//generally kept at 1
    float linear;//kept very small to cover large distances --- determines the max distance light can cover
    float quadratic;//even smaller than "linear"            --- determines the max distance light can cover (refer to table in general_notes.txt)
};
uniform Light light;

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
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuation;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position - FragPos);//point light/spotlight
    //vec3 lightDir = normalize(-light.direction);//directional light
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * attenuation; 

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * attenuation;

    //emission
    vec3 emission = texture(material.emission, TexCoords).rgb * attenuation;
            
    FragColor = vec4((ambient + diffuse + specular + emission), 1.0);
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