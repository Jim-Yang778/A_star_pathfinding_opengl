#version 330 core

uniform sampler2D u_scene;
uniform float exposure = 0.5f;

in vec2 v_tc;

out vec4 f_color;

void main(void) {
    const float gamma = 2.2;
    vec3 hdr_color = texture(u_scene, v_tc).rgb;

    // in chromatography, L means grayscale, or black and white.
    //    float L = (original_color.r + original_color.g + original_color.b) / 3;
    vec3 result = vec3(1.0) - exp(-hdr_color * exposure);
    result = pow(result, vec3(1.0 / gamma));
    f_color = vec4(result, 1.0f);
}
