#version 330 core

struct Material {
    sampler2D specular;//reflection intensity + texture of the object
    float shininess;//metalicity
    sampler2D  diffuse;//color + texture of the object
}; 
uniform Material material;

struct Light {
    vec3 position;//position of the light

    vec3 ambient;//generally set to a low value
    vec3 diffuse;//exact color we want light to have (often bright white)
    vec3 specular;//usually kept at 1 -> shining at full intensity
};
uniform Light light;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

void main(){
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); 

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
            
    FragColor = vec4((ambient + diffuse + specular), 1.0);
} 