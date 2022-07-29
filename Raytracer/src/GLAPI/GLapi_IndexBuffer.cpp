#include "GLapi_IndexBuffer.hpp"

void h3dgl::IndexBuffer::create(const uint32_t* data, unsigned int indexCount) {
    m_indexCount = indexCount;
    GLCALL(glGenBuffers(1, &buffer));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexCount, data, GL_STATIC_DRAW));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void h3dgl::IndexBuffer::destroy() {
    GLCALL(glDeleteBuffers(1, &buffer));
}

void h3dgl::IndexBuffer::bind() const {
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer));
}

void h3dgl::IndexBuffer::unbind() const {
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int h3dgl::IndexBuffer::IndexCount() const {
    return m_indexCount;
}

unsigned int h3dgl::IndexBuffer::size() {
    return m_indexCount * sizeof(uint32_t);
}