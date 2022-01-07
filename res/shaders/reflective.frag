#version 330 core

in vec3 normal;
in vec3 position;
//in vec3 v_tex_coord;

uniform vec3 camera_pos;
uniform mat4 u_view;
uniform samplerCube u_texture;
out vec4 f_color;

void main() {
    vec3 I      = normalize(camera_pos - position);
    vec3 viewR  = reflect(I, normalize(normal));
    vec3 worldR = inverse(mat3(u_view)) * viewR;
    f_color = vec4(texture(u_texture, worldR).rgb, 1.0);
}
