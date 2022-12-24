#version 330 core
struct Material {
    sampler2D specular;//reflection intensity + texture Of Object
    float shininess;//metalicity
    sampler2D  diffuse;//color + texture Of Object
    sampler2D  emission;//color + texture Of Object
}; 
uniform Material material;

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

uniform vec3 viewPos;

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