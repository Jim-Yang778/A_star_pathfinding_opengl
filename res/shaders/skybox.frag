#version 330 core

uniform samplerCube u_cubemap;

in vec3 v_texcoord;

out vec4 f_color;

void main(void) {
    f_color = texture(u_cubemap, v_texcoord);
}