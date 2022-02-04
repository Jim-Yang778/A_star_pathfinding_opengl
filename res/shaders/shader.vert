#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // 注意三个矩阵的乘法要从右往左乘 即PVM
    gl_Position = projection * view * model * aPos;
    // 我们可以使用inverse和transpose函数自己生成这个法线矩阵
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // 顶点位置属性乘以模型矩阵得到顶点在世界空间的坐标
    FragPos = vec3(model * aPos);
}
