#version 330 core
in layout (location = 0) vec3 aPos;
in layout (location = 1) vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}

