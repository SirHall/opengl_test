#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec4 vertPos;

void main()
{
    gl_Position = projection * view * model * position;
    vertPos     = position;
}