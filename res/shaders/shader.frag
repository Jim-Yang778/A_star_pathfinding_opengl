#version 330 core
out vec4 gl_FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;


void main() {
    //// 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(lightColor);

    //// 漫反射
    // 法线方向
    vec3 norm = normalize(Normal);
    // 光源方向向量 = 光源位置 和 片段位置 的向量差 （从片段指向光）
    vec3 lightDir = normalize(lightPos - FragPos);
    // norm和lightDir向量进行点乘，计算光源对当前片段实际的漫发射影响
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //// 镜面反射
    float specularStrength = 0.5;
    // 实现方向向量 = 视线位置 和 片段位置的向量差
    vec3 viewDir = normalize(viewPos - FragPos);
    // 等式右边，lightDir为光的入射方向，reflect函数要求第一项为从光源指向片段位置的向量
    vec3 reflectDir = reflect(-lightDir, norm);
    // 计算镜面分量
    int shininess = 64; //反光度 越高，反射光能力越强，散射越少
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    gl_FragColor = vec4(result, 1.0);
}
