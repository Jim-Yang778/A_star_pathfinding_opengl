#version 330 core

uniform mat4 u_model_view;
uniform mat4 u_projection;

layout (location = 0) in vec4 a_pos;
layout (location = 1) in vec2 a_tex_coord;

noperspective out vec2 v_tex_coord;

void main() {
    gl_Position = u_projection * u_model_view * a_pos;
    v_tex_coord = (gl_Position.xy / gl_Position.w) * 0.5 + vec2(0.5);
}
