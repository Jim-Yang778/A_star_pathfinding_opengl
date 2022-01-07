#version 330 core

uniform sampler2D u_scene;

in vec2 v_tc;

out vec4 f_color;

void main(void) {
    vec3 original_color = texture(u_scene, v_tc).rgb;

    // in chromatography, L means grayscale, or black and white.
//    float L = (original_color.r + original_color.g + original_color.b) / 3;

    f_color = vec4(original_color, 1);
}
