#pragma once

#include "GLapi_GLerror.h"
namespace h3dgl {
    class IndexBuffer {
    private:
        GLuint buffer;
        unsigned int m_indexCount;
    public:
        void create(const uint32_t* data, unsigned int indexCount);

        void destroy();

        void bind() const;

        void unbind() const;

        unsigned int IndexCount() const;

        unsigned int size();
    };

}