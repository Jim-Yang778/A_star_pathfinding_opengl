#include "renderer.hpp"

renderer_t make_renderer(const glm::mat4 &projection) {
    renderer_t renderer{};
    renderer.projection = projection;
    return renderer;
}

//void draw_model(const renderer_t &renderer,
//                Shader &shader,
//                const glm::mat4 &model,
//                const glm::mat4 &view,
//                const model_t &m) {
//
//    shader.setMat4("model", model);
//    shader.setMat4("view", view);
//    shader.setMat4("projection", renderer.projection);
//
//    for (const mesh_t &mesh: m.meshes) {
//        if (mesh.material_id != 0) {
//            const material_t &mtl = m.materials[0];
//            glBindTexture(GL_TEXTURE_2D, mtl.texture);
//            shader.setFloat("u_tex_factor", mtl.texture == 0 ? 1.f : 0.f);
//            // 第一层是0
//            shader.setInt("texture1", 0);
//            shader.setVec4("u_color", glm::vec4(1, 0.35, 0.5, 1));
//        } else {
//            shader.setFloat("u_tex_factor", 0.0f);
//            shader.setVec4("u_color", glm::vec4(1, 1, 1, 1));
//        }
//
//        glBindVertexArray(mesh.vao);
//        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);
//
//        glBindVertexArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindTexture(GL_TEXTURE_2D, 0);
//    }
//}

void draw_cube_normal(const renderer_t &renderer,
                Shader &shader,
                const glm::mat4 &model,
                const glm::mat4 &view,
                const glm::vec3 &lightPos,
                const model_t &m) {

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", glm::vec3(1, 1, 1));
        shader.setVec3("objectColor", glm::vec3(1, 0.35, 0.2));

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void draw_cube_light(const renderer_t &renderer,
                Shader &shader,
                const glm::mat4 &model,
                const glm::mat4 &view,
                const model_t &m) {

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", renderer.projection);

    for (const mesh_t &mesh: m.meshes) {
        shader.setVec4("u_color", glm::vec4(1, 1, 1, 1));

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glDrawArrays(GL_TRIANGLES, 0, mesh.nverts);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
