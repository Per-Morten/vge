#version 410 core
in layout (location = 0) vec3 aPosition;
in layout (location = 1) vec4 aColor;

out layout(location = 0) vec4 fColor;

uniform mat4 uVP;

void main()
{
    gl_Position = uVP * vec4(aPosition, 1.0);
    fColor = aColor;
}

