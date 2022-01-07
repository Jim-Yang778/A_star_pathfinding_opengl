#version 330 core

noperspective in vec2 v_tex_coord;

uniform sampler2D u_texture;

out vec4 f_color;

void main() {
    f_color = texture(u_texture, v_tex_coord);
}
