#ifndef LEARN_OPENGL_FILE_UTILS_HPP
#define LEARN_OPENGL_FILE_UTILS_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <string>
#include <iostream>
/*
 * 本文件用于texture,image等的加载和删除
 * */

struct image_t {
    int width;
    int height;
    int n_channels;
    void *data;
};

void delete_image(image_t &img) {
    stbi_image_free(img.data);
    img.data = nullptr;
    img.width = 0;
    img.height = 0;
    img.n_channels = 0;
}

image_t load_image(const std::string &filename, bool flip_vertical=true) {
    image_t img;
    stbi_set_flip_vertically_on_load(flip_vertical);
    img.data = stbi_load(filename.c_str(), &img.width, &img.height, &img.n_channels, 0);
    if (img.data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        throw std::runtime_error("Failed to load textures");
    }
    return img;
}



#endif //LEARN_OPENGL_FILE_UTILS_HPP
