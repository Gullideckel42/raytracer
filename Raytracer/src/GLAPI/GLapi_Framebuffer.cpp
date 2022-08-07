#include "GLapi_Framebuffer.hpp"


void h3dgl::Framebuffer::load(unsigned int width, unsigned int height, unsigned int attachmentCount, const GLenum* attachments, bool depthStencilBuffer, GLuint format) {
    GLCALL(glGenFramebuffers(1, &buffer));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, buffer));
    m_attachmentCount = attachmentCount;
    m_width = width;
    m_height = height;
    textureAttachments = new GLuint[attachmentCount];
    m_DepthStencilBuffer = depthStencilBuffer;
    GLCALL(glGenTextures(attachmentCount, textureAttachments));
    for (int i = 0; i < attachmentCount; i++) {
        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, textureAttachments[i]));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, textureAttachments[i], 0));
    }

    GLCALL(glDrawBuffers(attachmentCount, attachments));
    if (depthStencilBuffer) {
        GLCALL(glGenRenderbuffers(1, &rbo));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));
    }
    GLCALL(auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    while (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
    }
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void h3dgl::Framebuffer::destroy() {
    GLCALL(glDeleteTextures(m_attachmentCount, textureAttachments));
    if (m_DepthStencilBuffer) {
        GLCALL(glDeleteRenderbuffers(1, &rbo));
    }
    
    GLCALL(glDeleteFramebuffers(1, &buffer));
    delete[] textureAttachments;
}

void h3dgl::Framebuffer::bind() {
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, buffer));
}

void h3dgl::Framebuffer::unbind() {
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

glm::vec2 h3dgl::Framebuffer::getSize() {
    return glm::vec2(m_width, m_height);
}

unsigned int h3dgl::Framebuffer::getAmountOfAttachments() {
    return m_attachmentCount;
}

GLuint h3dgl::Framebuffer::getTextureAttachment(unsigned int target) {
    return textureAttachments[target];
}