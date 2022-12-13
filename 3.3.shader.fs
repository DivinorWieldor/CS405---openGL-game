#version 330 core
//fragment shader

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;//modifiable at runtime
uniform sampler2D texture2;

void main(){
    //FragColor = vec4(ourColor, 1.0f);
    FragColor = mix(texture(texture1, TexCoord),
                    texture(texture2, TexCoord), 0.2) //last value is mix%. If 0, returns only first input
                * vec4(ourColor, 1.0f);
}