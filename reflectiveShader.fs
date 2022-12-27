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

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform sampler2D shadowMap;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);

void main(){
    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;
  	vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 lightColor = vec3(0.3);

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 result = CalcDirLight(dirLight, norm, viewDir, shadow);//directional light
    for(int i = 0; i < NR_POINT_LIGHTS; i++)// phase 2: point lights
        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, shadow);    
    result += CalcSpotLight(spotLight, norm, fs_in.FragPos, viewDir, shadow);// phase 3: spot light

    FragColor = vec4(result, 1.0);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));//gamma correction - gamma = 2.2
}

//----------------------------------------------------------- Lighting Functions ----------------------------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow){//directional light
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    return ((light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords))) +
             light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, fs_in.TexCoords)) +
             light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, fs_in.TexCoords)));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow){//point light
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));// attenuation

    return ((light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords)) * attenuation) +
             light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, fs_in.TexCoords)) * attenuation +
             light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, fs_in.TexCoords)) * attenuation);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow){//spot light
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));// attenuation
    
    //                                                theta-------------------------------------                        epsilon-------------------------
    float attenuationIntensity = attenuation * clamp((dot(lightDir, normalize(-light.direction)) - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);// spotlight intensity

    return ((light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords)) * attenuationIntensity) +
             light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, fs_in.TexCoords)) * attenuationIntensity +
             light.specular * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), material.shininess) * vec3(texture(material.specular, fs_in.TexCoords)) * attenuationIntensity);
}

float ShadowCalculation(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;// perform perspective divide
    projCoords = projCoords * 0.5 + 0.5;// transform to [0,1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float currentDepth = projCoords.z;// get depth of current fragment from light's perspective
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;// check whether current frag pos is in shadow

    return shadow;
}