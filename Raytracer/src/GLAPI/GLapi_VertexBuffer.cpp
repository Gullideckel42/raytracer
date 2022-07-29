#include "GLapi_VertexBuffer.hpp"
void h3dgl::VertexBuffer::create(const void* data, unsigned int vertexCount) {
    //std::cout << "Creating... Vertexbuffer" << std::endl;
    m_vertexCount = vertexCount;
    GLCALL(glGenVertexArrays(1, &vertexArray));
    GLCALL(glBindVertexArray(vertexArray));
    GLCALL(glGenBuffers(1, &buffer));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(h3dgl::Vertex), data, GL_STATIC_DRAW));
    GLCALL(glEnableVertexAttribArray(0));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(h3dgl::Vertex), (const void*)offsetof(struct h3dgl::Vertex, x)));
    GLCALL(glEnableVertexAttribArray(1));
    GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(h3dgl::Vertex), (const void*)offsetof(struct h3dgl::Vertex, u)));
    GLCALL(glEnableVertexAttribArray(2));
    GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(h3dgl::Vertex), (const void*)offsetof(struct h3dgl::Vertex, nx)));

    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCALL(glBindVertexArray(0));
    //std::cout << "Created Vertexbuffer" << std::endl;
}

void h3dgl::VertexBuffer::destroy() {
    GLCALL(glDeleteBuffers(1, &buffer));
    GLCALL(glDeleteVertexArrays(1, &vertexArray));
}

void h3dgl::VertexBuffer::bind() const {
    GLCALL(glBindVertexArray(vertexArray));
}

void h3dgl::VertexBuffer::unbind() const {
    GLCALL(glBindVertexArray(0));
}

unsigned int h3dgl::VertexBuffer::VertexCount() const {
    return m_vertexCount;
}

unsigned int h3dgl::VertexBuffer::size() {
    return m_vertexCount * sizeof(h3dgl::Vertex);
}