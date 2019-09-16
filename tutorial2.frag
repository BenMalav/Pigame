#version 300 es

precision mediump float; 
in vec3 ex_Color;

out vec3 color;

void main(void) 
{
    color = ex_Color;
}