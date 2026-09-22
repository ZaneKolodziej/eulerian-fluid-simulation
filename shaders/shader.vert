#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoordinate;
layout(location = 2) in vec4 vertexColor;

out vec2 fragTexCoordinate;
out vec4 fragColor;

uniform mat4 mvp;

void main() 
{
    fragTexCoordinate = vertexTexCoordinate;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}