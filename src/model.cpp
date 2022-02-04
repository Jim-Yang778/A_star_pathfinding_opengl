#include "model.hpp"
#include "file_utils.hpp"

// helper function
mesh_t make_mesh(
        const std::vector<glm::vec4> &positions,
        const std::vector<glm::vec2> &texcoords,
        const std::vector<glm::vec3> &normals,
        int material_id
) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLsizeiptr pos_sz = positions.size() * sizeof(glm::vec4);
    GLsizeiptr tc_sz = texcoords.size() * sizeof(glm::vec2);
    GLsizeiptr nrm_sz = normals.size() * sizeof(glm::vec3);
    glBufferData(
            GL_ARRAY_BUFFER,
            pos_sz + tc_sz + nrm_sz,
            nullptr,
            GL_STATIC_DRAW
    );
    // 分批处理顶点属性
    glBufferSubData(GL_ARRAY_BUFFER, 0, pos_sz, positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, pos_sz, tc_sz, texcoords.data());
    glBufferSubData(GL_ARRAY_BUFFER, pos_sz + tc_sz, nrm_sz, normals.data());
    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) pos_sz);
    // normal attribute
    if (nrm_sz != 0) {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) (pos_sz + tc_sz));
    }

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (nrm_sz != 0) glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return mesh_t{ vao, vbo,  (GLuint)positions.size(), material_id };
}

GLuint load_texture(const std::string &path) {
    image_t tex_img = load_image(path);
    GLint fmt = tex_img.n_channels == 3 ? GL_RGB : GL_RGBA;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, tex_img.width, tex_img.height, 0, fmt, GL_UNSIGNED_BYTE, tex_img.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete_image(tex_img);

    return tex;
}

model_t generate_cube(const float &width,
                      const float &height,
                      const float &depth,
                      const bool &tex_cover,
                      const std::string& path) {
    float w = width;
    float h = height;
    float d = depth;

    float hw, hh, hd;
    if (tex_cover) {
        hw = 1;
        hh = 1;
        hd = 1;
    } else {
        hw = w / 2;
        hh = h / 2;
        hd = d / 2;
    }

    std::vector<vertex_t> verts = {
            // front face
            {{ -w, -h, d },  { 1,  0,  1 },  { 0,  0 }},
            {{ w,  -h, d },  { 1,  0,  1 },  { hw, 0 }},
            {{ w,  h,  d },  { 0,  0,  1 },  { hw, hh }},
            {{ -w, h,  d },  { 0,  0,  1 },  { 0,  hh }},

            // right face
            {{ w,  -h, d },  { 1,  0,  0 },  { 0,  0 }},
            {{ w,  -h, -d }, { 1,  0,  0 },  { hd, 0 }},
            {{ w,  h,  -d }, { 1,  0,  0 },  { hd, hh }},
            {{ w,  h,  d },  { 1,  0,  0 },  { 0,  hh }},

            // back face
            {{ w,  -h, -d }, { 0,  0,  -1 }, { 0,  0 }},
            {{ -w, -h, -d }, { 0,  0,  -1 }, { hw, 0 }},
            {{ -w, h,  -d }, { 0,  0,  -1 }, { hw, hh }},
            {{ w,  h,  -d }, { 0,  0,  -1 }, { 0,  hh }},

            // left face
            {{ -w, -h, -d }, { -1, 0,  0 },  { 0,  0 }},
            {{ -w, -h, d },  { -1, 0,  0 },  { hd, 0 }},
            {{ -w, h,  d },  { -1, 0,  0 },  { hd, hh }},
            {{ -w, h,  -d }, { -1, 0,  0 },  { 0,  hh }},

            // top face
            {{ -w, h,  d },  { 0,  1,  0 },  { 0,  0 }},
            {{ w,  h,  d },  { 0,  1,  0 },  { hw, 0 }},
            {{ w,  h,  -d }, { 0,  1,  0 },  { hw, hd }},
            {{ -w, h,  -d }, { 0,  1,  0 },  { 0,  hd }},

            // bottom face
            {{ -w, -h, -d }, { 0,  -1, 0 },  { 0,  0 }},
            {{ w,  -h, -d }, { 0,  -1, 0 },  { hw, 0 }},
            {{ w,  -h, d },  { 0,  -1, 0 },  { hw, hd }},
            {{ -w, -h, d },  { 0,  -1, 0 },  { 0,  hd }},
    };
    std::vector<GLuint> indices = {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
    };

    std::vector<glm::vec4> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    auto push_vert = [&](GLuint i) {
        const auto& v = verts[i];
        positions.emplace_back(v.pos, 1);
        texcoords.emplace_back(v.tc);
        normals.emplace_back(v.nrm);
    };

    for (GLuint i = 0; i <= indices.back(); i = i + 4) {
        push_vert(i);
        push_vert(i + 1);
        push_vert(i + 2);

        push_vert(i);
        push_vert(i + 2);
        push_vert(i + 3);
    }

    std::vector<mesh_t> meshes;
    if (tex_cover) {
        meshes.emplace_back(make_mesh(positions, texcoords, normals, 1));
    } else {
        meshes.emplace_back(make_mesh(positions, texcoords, normals, 0));
    }

    std::vector<material_t> materials;
    GLuint tex = 0;
    if (tex_cover) {
        tex = load_texture(path);
        materials.emplace_back(material_t { tex, glm::vec4(1, 1, 1, 1) });
    } else {
        materials.emplace_back(material_t { tex, glm::vec4(1, 1, 1, 1) });
    }
    return { meshes, materials };
}

model_t generate_sphere(const int &slices, const int &stacks) {
    std::vector<vertex_t> sverts;
    for (int i = 0; i <= stacks; ++i) {
        double phi = glm::pi<double>() * i / (double) stacks;

        for (int j = 0; j <= slices; ++j) {
            double theta = 2 * glm::pi<double>() * j / (double) slices;

            glm::vec3 p = { std::cos(theta) * std::sin(phi), std::cos(phi), std::sin(theta) * std::sin(phi) };
            glm::vec3 normal = glm::normalize(p);
            glm::vec2 tc = { (float) -j / (float) slices, (float) i / (float) stacks };

            sverts.emplace_back(vertex_t{ p, normal, tc });
        }
    }
    std::vector<glm::vec4> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    auto push_vert = [&](int i) {
        const auto& v = sverts[i];
        positions.emplace_back(glm::vec4(v.pos,1));
        texcoords.emplace_back(v.tc);
        normals.emplace_back(v.nrm);
    };
    for (int i = 0; i < slices * stacks + stacks; ++i) {
        push_vert(i);
        push_vert(i + slices + 1);
        push_vert(i + slices);

        push_vert(i + slices + 1);
        push_vert(i);
        push_vert(i + 1);
    }

    std::vector<mesh_t> meshes;
    meshes.emplace_back(make_mesh(positions, texcoords, normals, 0));

    std::vector<material_t> materials;
    materials.emplace_back(material_t { 0, glm::vec4(1, 1, 1, 1) });

    return { meshes, materials };
}

void destroy_obj(model_t &m) {
    for (mesh_t &mesh: m.meshes) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);

        mesh.vbo = mesh.vao = 0;
        mesh.nverts = 0;
    }

    for (material_t &mat: m.materials) {
        glDeleteTextures(1, &mat.texture);
        mat.texture = 0;
    }
}