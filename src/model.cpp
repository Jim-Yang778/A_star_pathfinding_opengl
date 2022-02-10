#include "model.hpp"
#include "file_utils.hpp"

// helper function
mesh_t make_mesh(
        const std::vector<glm::vec4> &positions,
        const std::vector<glm::vec2> &texcoords,
        const std::vector<glm::vec3> &normals,
        const int material_id = -1
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

model_t generate_cube(const volume_param_t &geo_params,
                      const bool &tex_cover,
                      const material_param_t &mat_params,
                      glm::vec4 color) {
    float w = geo_params.width;
    float h = geo_params.height;
    float d = geo_params.depth;

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
    meshes.emplace_back(make_mesh(positions, texcoords, normals));

    std::vector<material_t> materials;
    if (tex_cover) {
        GLuint diffuse_map = load_texture(mat_params.diffuse_map);
        GLuint specular_map = load_texture(mat_params.specular_map);
        materials.emplace_back(material_t { glm::vec4(1), diffuse_map, specular_map });
    } else {
        materials.emplace_back(material_t { color });
    }
    return { meshes, materials };
}

model_t generate_sphere(const sphere_param_t &geo_params,
                        glm::vec4 color) {
    int stacks = geo_params.stacks;
    int slices = geo_params.slices;
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
    meshes.emplace_back(make_mesh(positions, texcoords, normals));

    std::vector<material_t> materials;
    materials.emplace_back(material_t { color });

    return { meshes, materials };
}

model_t load_obj(const std::string &path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string str_warn, str_err;
    std::string mtl_base = path.substr(0, path.find_last_of('/') + 1);

    bool did_load = tinyobj::LoadObj(&attrib, &shapes, &materials, &str_warn, &str_err, path.c_str(), mtl_base.c_str());
    if (!(did_load && str_warn.empty() && str_err.empty())) {
        throw std::runtime_error(str_warn + str_err);
    }

    // load the meshes
    std::vector<mesh_t> meshes;
    for (const tinyobj::shape_t &shape: shapes) {
        // every 3 indices makes a face, quads etc.
        // In .obj files, the vertices are triangulated by default
        std::vector<glm::vec4> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        for (const tinyobj::index_t &index: shape.mesh.indices) {
            float *pos = &attrib.vertices[3 * index.vertex_index];
            float *nrm = &attrib.normals[3 * index.normal_index];
            float *tc = &attrib.texcoords[2 * index.texcoord_index];

            positions.emplace_back(pos[0], pos[1], pos[2], 1);
            normals.emplace_back(nrm[0], nrm[1], nrm[2]);
            texcoords.emplace_back(tc[0], tc[1]);
        }
        int material = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];

        meshes.emplace_back(make_mesh(positions, texcoords, normals, material));
    }

    // load the materials
    std::vector<material_t> mats;
    for (const tinyobj::material_t &mat: materials) {
        GLuint tex = mat.diffuse_texname.empty() ? 0 : load_texture(mtl_base + mat.diffuse_texname);
        GLuint spec = mat.specular_texname.empty() ? 0 : load_texture(mtl_base + mat.specular_texname);
        glm::vec4 color = { mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1 };

        mats.emplace_back(material_t{ color, tex, spec });
    }

    return model_t{ meshes, mats };
}

void destroy_obj(model_t &m) {
    for (mesh_t &mesh: m.meshes) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);

        mesh.vbo = mesh.vao = 0;
        mesh.nverts = 0;
    }

    for (material_t &mat: m.materials) {
        glDeleteTextures(1, &mat.diffuse_map);
        glDeleteTextures(1, &mat.specular_map);
        mat.diffuse_map = 0;
        mat.specular_map = 0;
    }
}