#version 330 core

in vec3 position;
in vec3 in_normal;
in vec2 v_tex_coord;

uniform sampler2D u_texture;
//uniform float exposure = 1.0f;
uniform float u_tex_factor;
uniform vec4 u_color;

out vec4 f_color;

uniform vec3 lightPos = {0.0f, -1.0f, 0.0f};
uniform vec3 viewPos;

void main() {
    const float gamma = 2.2;
    vec3 color = texture(u_texture, v_tex_coord).rgb;

    // ambient
    vec3 ambient = 0.8 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - position);
    vec3 normal = normalize(in_normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.3;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    f_color = vec4(ambient + diffuse + specular, 1.0);

    f_color = mix(u_color, f_color, u_tex_factor);
}
