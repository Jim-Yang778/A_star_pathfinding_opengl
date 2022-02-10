#ifndef LEARN_OPENGL_MODEL_HPP
#define LEARN_OPENGL_MODEL_HPP

#include <vector>
#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <tiny_obj_loader.h>

/*
 * 本文件创建立体模型cube, sphere
 * 并完成顶点着色器部分的装载(?)
 * */


struct vertex_t {
    glm::vec3 pos;
    glm::vec3 nrm;
    glm::vec2 tc;
};

struct mesh_t {
    GLuint vao;
    GLuint vbo;
    GLuint nverts;
    int material_id;
};

struct material_t {
    glm::vec4 color;
    GLuint diffuse_map = 0;
    GLuint specular_map = 0;
};

struct model_t {
    std::vector<mesh_t> meshes;
    std::vector<material_t> materials;
};

// 立方体参数
struct volume_param_t {
    float width = 1;
    float height = 1;
    float depth = 1;
};

// 球体参数
struct sphere_param_t {
    int slices = 16;
    int stacks = 16;
};

struct material_param_t {
    std::string diffuse_map{};
    std::string specular_map{};
};

GLuint load_texture(const std::string &path);

model_t generate_cube(const volume_param_t &geo_params,
                      const bool &tex_cover,
                      const material_param_t &mat_params,
                      glm::vec4 color = {1, 1, 1, 1});
model_t generate_sphere(const sphere_param_t &geo_params,
                        glm::vec4 color = {1, 1, 1, 1});

model_t load_obj(const std::string &path);

void destroy_obj(model_t &m);

#endif //LEARN_OPENGL_MODEL_HPP
