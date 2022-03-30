#include <stack>
#include "renderer.hpp"

renderer_t make_renderer(const glm::mat4 &projection) {
    renderer_t renderer{};
    renderer.projection = projection;
    return renderer;
}

// 渲染带颜色的方块
void draw_cube_color(const renderer_t &renderer,
                const Shader &shader,
                const glm::mat4 &model,
                const glm::mat4 &view,
                const std::vector<glm::vec3> &lightPos,
                const glm::vec3 &viewPos,
                const model_t &m) {

    shader.use();

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        const material_t &mtl = m.materials[0];
        shader.setVec3("viewPos", viewPos);
        shader.setVec3("material.ambient",  mtl.color);
        shader.setVec3("material.diffuse",  mtl.color);
        shader.setVec3("material.specular", mtl.color);
        shader.setFloat("material.shininess", 16.0f);
        // 点光源_1
        shader.setVec3("pointLight[0].position", lightPos[0]);
        shader.setVec3("pointLight[0].ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("pointLight[0].diffuse", 0.5f, 0.5f, 0.5f);
        shader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight[0].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[0].linear",    0.09f);
        shader.setFloat("pointLight[0].quadratic", 0.032f);
        // 点光源_2
        shader.setVec3("pointLight[1].position", lightPos[1]);
        // 根据两个点光源进行修改..
        shader.setVec3("pointLight[1].ambient", 0.0f,0.134f,0.082f);
        shader.setVec3("pointLight[1].diffuse", 0.0f, 0.335f, 0.205f);
        shader.setVec3("pointLight[1].specular", 0.0f,0.67f,0.41f);

        shader.setFloat("pointLight[1].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[1].linear",    0.09f);
        shader.setFloat("pointLight[1].quadratic", 0.032f);
        // 直线光
        shader.setVec3("dirLight.direction", dirLight);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// 渲染有纹理的方块，本项目中特指墙体
void draw_cube_texture(const renderer_t &renderer,
                       const Shader &shader,
                       const glm::mat4 &model,
                       const glm::mat4 &view,
                       const std::vector<glm::vec3> &lightPos,
                       const glm::vec3 &viewPos,
                       const model_t &m) {
    shader.use();

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        const material_t &mtl = m.materials[0];
        shader.setVec3("viewPos", viewPos);
        // 纹理相关
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mtl.diffuse_map);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mtl.specular_map);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 64.0f);
        // 点光源_1
        shader.setVec3("pointLight[0].position", lightPos[0]);
        shader.setVec3("pointLight[0].ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("pointLight[0].diffuse", 0.5f, 0.5f, 0.5f);
        shader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight[0].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[0].linear",    0.09f);
        shader.setFloat("pointLight[0].quadratic", 0.032f);
        // 点光源_2
        shader.setVec3("pointLight[1].position", lightPos[1]);
        shader.setVec3("pointLight[1].ambient", 0.0f,0.134f,0.082f);
        shader.setVec3("pointLight[1].diffuse", 0.0f, 0.335f, 0.205f);
        shader.setVec3("pointLight[1].specular", 0.0f,0.67f,0.41f);
        shader.setFloat("pointLight[1].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[1].linear",    0.09f);
        shader.setFloat("pointLight[1].quadratic", 0.032f);
        // 直线光
        shader.setVec3("dirLight.direction", dirLight);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// 渲染发光体
void draw_light(const renderer_t &renderer,
                const Shader &shader,
                const glm::mat4 &model,
                const glm::mat4 &view,
                const model_t &m) {

    shader.use();

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        shader.setVec4("u_color", m.materials[0].color);

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// 渲染模型，本项目中特指单车
void draw_obj(const renderer_t &renderer,
                       const Shader &shader,
                       const glm::mat4 &model,
                       const glm::mat4 &view,
                       const std::vector<glm::vec3> &lightPos,
                       const glm::vec3 &viewPos,
                       const model_t &m) {
    shader.use();

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        const material_t &mtl = m.materials[mesh.material_id];
        shader.setVec3("viewPos", viewPos);
        // 纹理相关
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mtl.diffuse_map);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mtl.specular_map);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 64.0f);
        // 点光源_1
        shader.setVec3("pointLight[0].position", lightPos[0]);
        shader.setVec3("pointLight[0].ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("pointLight[0].diffuse", 0.5f, 0.5f, 0.5f);
        shader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight[0].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[0].linear",    0.09f);
        shader.setFloat("pointLight[0].quadratic", 0.032f);
        // 点光源_2
        shader.setVec3("pointLight[1].position", lightPos[1]);
        shader.setVec3("pointLight[1].ambient", 0.0f,0.134f,0.082f);
        shader.setVec3("pointLight[1].diffuse", 0.0f, 0.335f, 0.205f);
        shader.setVec3("pointLight[1].specular", 0.0f,0.67f,0.41f);
        shader.setFloat("pointLight[1].constant",  1.0f); //点光源衰减相关
        shader.setFloat("pointLight[1].linear",    0.09f);
        shader.setFloat("pointLight[1].quadratic", 0.032f);
        // 直线光
        shader.setVec3("dirLight.direction", dirLight);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}