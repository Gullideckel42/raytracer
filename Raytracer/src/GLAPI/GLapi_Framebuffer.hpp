#pragma once
#include "GLapi_GLerror.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace h3dgl {
    class Framebuffer {
    private:
        GLuint buffer;
        GLuint rbo;
        GLuint* textureAttachments;
        unsigned int m_attachmentCount;
        unsigned int m_width;
        unsigned int m_height;
        bool m_DepthStencilBuffer;
    public:

        void load(unsigned int width, unsigned int height, unsigned int attachmentCount, const GLenum* attachments, bool depthStencilBuffer, GLuint format=GL_RGBA16F, GLuint type=GL_FLOAT);

        void destroy();

        void bind();

        void unbind();

        glm::vec2 getSize();

        bool saveToPng(const std::string& path, unsigned int target);

        unsigned int getAmountOfAttachments();

        GLuint getTextureAttachment(unsigned int target);
    };
}