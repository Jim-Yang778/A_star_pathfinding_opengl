#version 330 core

layout (location = 0) in vec4 a_pos;
layout (location = 1) in vec2 a_tc;

out vec2 v_tc;

void main(void) {
    v_tc = a_tc;

    gl_Position = a_pos;
}
