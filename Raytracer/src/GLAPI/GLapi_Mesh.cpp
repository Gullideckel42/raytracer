#include "GLapi_Mesh.hpp"
#include "ThirdParty/OBJ_Loader.h"
void h3dgl::Mesh::loadFromFile(std::string path, unsigned int meshIndex) {
	// Uses OBJ_Loader.h
	objl::Loader loader;
	if (!loader.LoadFile(path)) {
		rt_error("Could not load mesh (", path, ")");
		return;
	}
	
	std::vector <objl::Mesh> meshes = loader.LoadedMeshes;
	rt_info(path, ": ", meshes.size(), " Meshes found");
	std::vector <uint32_t> l_indices;

	std::vector<h3dgl::Vertex> l_vertices;
	//int z = meshes.size() == 2 ? 1 : 0;
	if (!(meshIndex < meshes.size())) {
		rt_error(path, " has only ", meshes.size(), " meshes");
		return;
	}

	objl::Mesh& m = meshes[meshIndex];
	for (int i = 0; i < m.Vertices.size(); i++) {
		l_vertices.push_back({
			m.Vertices.at(i).Position.X,
			m.Vertices.at(i).Position.Y,
			m.Vertices.at(i).Position.Z,
			m.Vertices.at(i).TextureCoordinate.X,
			m.Vertices.at(i).TextureCoordinate.Y,
			m.Vertices.at(i).Normal.X,
			m.Vertices.at(i).Normal.Y,
			m.Vertices.at(i).Normal.Z
			});
	}
	for (int i = 0; i < m.Indices.size(); i++) {
		l_indices.push_back(m.Indices[i]);
	}
	
	load(l_vertices.data(), l_vertices.size(), l_indices.data(), l_indices.size());
	rt_info("Loaded mesh (", vertexCount(), " vertices, ", indexCount(), " indices");
}

void h3dgl::Mesh::load(const h3dgl::Vertex* vertexData, unsigned int vertexCount, const uint32_t* indexData, unsigned int indexCount) {
	//std::cout << "Create mesh" << std::endl;
	vertices.create(vertexData, vertexCount);
	//std::cout << "Created mesh" << std::endl;
    indices.create(indexData, indexCount);
	m_loaded = true;
}

void h3dgl::Mesh::bind() const {
	if (!m_loaded) return;
    vertices.bind();
    indices.bind();
}

void h3dgl::Mesh::unbind() const {
    indices.unbind();
    vertices.unbind();
}

void h3dgl::Mesh::destroy() {
    indices.destroy();
    vertices.destroy();
}

unsigned int h3dgl::Mesh::vertexCount() const {
    return vertices.VertexCount();
}

unsigned int h3dgl::Mesh::indexCount() const {
    return indices.IndexCount();
}