#pragma once

#include <string>
#include "GLapi_IndexBuffer.hpp"
#include "GLapi_VertexBuffer.hpp"
namespace h3dgl {


    struct Submesh
    {
        VertexBuffer vertices;
        IndexBuffer indices;

        unsigned int submesh_index;
    };

    class Mesh {
    private:
        //Submesh* submeshes;
        //unsigned int submesh_count;
        VertexBuffer vertices;
        IndexBuffer indices;
        bool m_loaded = false;
    public:

        void loadFromFile(std::string path, unsigned int meshIndex = 0);

        void load(const h3dgl::Vertex* vertexData, unsigned int vertexCount, const uint32_t* indexData, unsigned int indexCount);

        void bind() const;

        void unbind() const;

        void destroy();

        unsigned int vertexCount() const;

        unsigned int indexCount() const;

        bool loaded() { return m_loaded; }

    };
}