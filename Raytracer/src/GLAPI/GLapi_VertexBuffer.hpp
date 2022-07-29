#pragma once
#include "GLapi_GLerror.h"
#include "GLapi_Vertex.hpp"
namespace h3dgl {

    class VertexBuffer {
    private:
        GLuint vertexArray = 0;
        GLuint buffer;
        unsigned int m_vertexCount;
    public:
        void create(const void* data, unsigned int vertexCount);

        void destroy();

        void bind() const;

        void unbind() const;

        unsigned int VertexCount() const;

        unsigned int size();

    };
}