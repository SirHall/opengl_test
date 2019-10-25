#version 330 core

out vec4 color;

in vec4 vertPos;

uniform vec4 u_Color;

void main() { color = (vertPos * 0.5) + 0.5; }