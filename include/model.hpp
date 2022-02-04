#ifndef LEARN_OPENGL_MODEL_HPP
#define LEARN_OPENGL_MODEL_HPP

#include <vector>
#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

/*
 * 本文件创建立体模型cube, sphere
 * 并完成顶点着色器部分的装载(?)
 * */


/*
 * 保存:
 * position
 * normal
 * texture coordinate
 * */
struct vertex_t {
    glm::vec3 pos;
    glm::vec3 nrm;
    glm::vec2 tc;
};

/*
 * 保存顶点着色器需要的变量
 * */
struct mesh_t {
    GLuint vao;
    GLuint vbo;
    GLuint nverts;
    int material_id;
};

struct material_t {
    GLuint texture;
    glm::vec4 color;
};

struct model_t {
    std::vector<mesh_t> meshes;
    std::vector<material_t> materials;
};

model_t generate_cube(const float &width,
                      const float &height,
                      const float &depth,
                      const bool &tex_cover,
                      const std::string& path);
model_t generate_sphere(const int &slices = 16, const int &stacks = 32);

void destroy_obj(model_t &m);

#endif //LEARN_OPENGL_MODEL_HPP
