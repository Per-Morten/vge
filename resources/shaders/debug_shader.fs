#version 410 core

in layout(location = 0) vec4 fColor;

out vec4 FragColor;

void main()
{
    FragColor = fColor;
}
