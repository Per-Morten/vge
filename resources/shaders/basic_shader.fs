#version 410 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform vec4 u_color;
uniform int u_test;

uniform float far = 100.0;
uniform float near = 0.1;


void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);
    ///FragColor = texture(u_texture0, texCoord);
    //FragColor = u_color;
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    FragColor = mix(texture(u_texture0, texCoord), texture(u_texture1, texCoord), 0.2);
    //float z = (2 * near) / (far + near - gl_FragCoord.z * (far - near));
    //FragColor = vec4(vec3(z), 1.0);
}
