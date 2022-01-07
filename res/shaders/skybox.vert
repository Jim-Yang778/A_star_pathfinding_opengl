#version 330 core

uniform mat4 u_v;
uniform mat4 u_p;

layout (location = 0) in vec4 a_pos;

out vec3 v_texcoord;


void main(void) {
    v_texcoord = a_pos.xyz;

    gl_Position = (u_p * mat4(mat3(u_v)) * a_pos);
}