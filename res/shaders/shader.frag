#version 330 core
out vec4 gl_FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;


void main() {
    ///// 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(lightColor);

    //// 漫反射
    // 法线方向
    vec3 norm = normalize(Normal);
    // 光源方向向量 = 光源位置 和 片段位置 的向量差
    vec3 lightDir = normalize(lightPos - FragPos);
    // norm和lightDir向量进行点乘，计算光源对当前片段实际的漫发射影响
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    gl_FragColor = vec4(result, 1.0);
}
