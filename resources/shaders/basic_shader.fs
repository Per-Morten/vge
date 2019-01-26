#version 410 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform vec4 u_color;
uniform int u_test;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);
    ///FragColor = texture(u_texture0, texCoord);
    FragColor = u_color;
    //FragColor = mix(texture(u_texture0, texCoord), texture(u_texture1, texCoord), 0.2);
}
