#ifndef LEARN_OPENGL_RENDERER_HPP
#define LEARN_OPENGL_RENDERER_HPP

#include <glad/glad.h>
#include <glm/ext.hpp>
#include "model.hpp"
#include "shader.hpp"

struct renderer_t {
    glm::mat4 projection;
    glm::mat4 view;
};

renderer_t make_renderer(const glm::mat4 &projection);

/**
 * 画一个model_t类型的m
 *
 * @param renderer renderer_t类型的结构体，存着渲染需要的数据
 * @param model 模型矩阵
 * @param view 观察矩阵
 * @param m 模型本身
 */
void draw_model(const renderer_t &renderer,
                Shader &shader,
                const glm::mat4 &model,
                const glm::mat4 &view,
                const model_t &m);

void draw_cube_light(const renderer_t &renderer,
                     Shader &shader,
                     const glm::mat4 &model,
                     const glm::mat4 &view,
                     const model_t &m);

void draw_cube_normal(const renderer_t &renderer,
                      Shader &shader,
                      const glm::mat4 &model,
                      const glm::mat4 &view,
                      const glm::vec3 &lightPos,
                      const model_t &m);

#endif //LEARN_OPENGL_RENDERER_HPP
