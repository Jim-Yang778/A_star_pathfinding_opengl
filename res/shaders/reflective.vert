#version 330 core

uniform mat4 u_model_view;
//uniform mat4 view;
uniform mat4 u_projection;

layout (location = 0) in vec4 a_pos;
layout (location = 1) in vec2 a_tex_coord;
layout (location = 2) in vec3 a_normal;

out vec3 normal;
out vec3 position;

void main() {
    normal = mat3(transpose(inverse(u_model_view))) * a_normal;
    position = vec3(u_model_view * a_pos);
    gl_Position = u_projection * vec4(position, 1.0);
}