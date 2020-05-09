//
// Created by Admin on 09.05.2020.
//

#include "utilities.h"
#include <GL/glew.h>
#include "stb_image.h"
#include <iostream>

void load_texture(const char * file_string, GLenum n_texture_unit, GLuint & textureID) {

    // скачиваем картинку
    int im_width, im_height, im_channels;
    unsigned char * im_data = stbi_load(file_string, &im_width, &im_height, &im_channels, 0);
    std::printf("Texture loading: %s Image: width: %d, height %d, channels %d\n",file_string, im_width, im_height, im_channels);

    //делаем текстуру
    glGenTextures(1, &textureID);

    // активизируем переданный текстурный юнит
    glActiveTexture(n_texture_unit);
    // привязываем соответствующий идентификатор к точке GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // наполняем текстурку данными
    if(im_channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im_width, im_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, im_data);
    } else if (im_channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im_width, im_height, 0, GL_RGB, GL_UNSIGNED_BYTE, im_data);
    } else {
        std::printf("Texture loading: Wrong channels count %d\n", im_channels);
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::printf("Texture loading: There is OpenGL error: %d\n", err);
    }
    // делаем мипмапы
    glGenerateMipmap(GL_TEXTURE_2D);

    // очищаем данные
    stbi_image_free(im_data);

    std::printf("Texture loading: Texture from %s is loaded\n", file_string);
}

