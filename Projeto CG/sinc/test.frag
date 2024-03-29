#version 330 core

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D myTextureSampler;
uniform vec3 lightPos;         // Example light position
uniform vec3 lightColor;       // Example light color
uniform vec3 viewPos;          // Example camera position

uniform float ambientStrength;  // Example ambient strength
uniform float specularStrength; // Example specular strength
uniform float shininess;        // Example shininess

void main(){
    // Ambient component
    vec3 ambient = ambientStrength * texture(myTextureSampler, UV).xyz;

    // Diffuse component
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Final color with texture
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}