#include "GLapi_Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

void h3dgl::Texture::load(const std::string& path) {
    if (m_loaded) return;
	stbi_set_flip_vertically_on_load(true);
	auto texBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);
    std::cout << "Path: " << path.c_str() <<  ",Width: " << m_width << ", Height: " << m_height << "| bpp: " 
        << m_bitsPerPixel << std::endl;
	GLCALL(glGenTextures(1, &texture));
    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, /*TODO*/ GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer));
    GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    if (texBuffer) {
        stbi_image_free(texBuffer);
        m_loaded = true;
        m_path = path;
    }
}

void h3dgl::Texture::destroy() {
    if (!m_loaded) return;
    GLCALL(glDeleteTextures(1, &texture));
    texture = 0;
    m_width = 0;
    m_height = 0;
    m_bitsPerPixel = 0;
    m_loaded = false;
    std::cout << "Deleted texture: " << m_path << std::endl;
}

bool h3dgl::Texture::isLoaded() {
    return m_loaded;
}

void h3dgl::Texture::bind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
}

void h3dgl::Texture::unbind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}

int h3dgl::Texture::getWidth() const {
    return m_width;
}

int h3dgl::Texture::getHeight() const {
    return m_height;
}

void h3dgl::Cubemap::load(
    const std::string& pX,
    const std::string& pY,
    const std::string& pZ,
    const std::string& nX,
    const std::string& nY,
    const std::string& nZ
)
{
    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));

    std::string files[] = { pX, nX, pY, nY, pZ, nZ };
    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < 6; i++) {

        auto texBuffer = stbi_load(files[i].c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);
        GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, m_width, m_height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer));
        if (texBuffer) {
            stbi_image_free(texBuffer);
        }
        else {
           std::cout << "Texture could not be loaded: Cubemap" << std::endl;
        }
    }
    
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP , 0));
    m_loaded = true;
}

void h3dgl::Cubemap::bind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
}

void h3dgl::Cubemap::unbind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

GLenum h3dgl::Texture::getId() {
    return texture;
}


