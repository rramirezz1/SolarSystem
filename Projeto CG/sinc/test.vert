#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 UV;
out vec3 Normal;
out vec3 FragPos;


void main(){
    FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    Normal = mat3(transpose(inverse(model))) * normalize(vec3(vertexPosition_modelspace)); // normal no espaco do mundo

    UV = vertexUV;
}