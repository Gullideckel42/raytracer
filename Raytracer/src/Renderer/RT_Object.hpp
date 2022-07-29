#pragma once
#include "../Core.hpp"
#include "RT_Material.hpp"
RT_START


class Object
{
public:

	RT_Result create(std::string name, GL_ Mesh* mesh, RT_ Material mat);

	void bind();

	void unbind();

	unsigned int IndexCount() const { return m_mesh->indexCount(); }
	unsigned int VertexCount() const { return m_mesh->vertexCount(); }

	glm::mat4& getTransform() { return m_transform; }
	glm::vec3& getPosition() { return m_position; }
	glm::vec3& getRotation() { return m_rotation; }
	glm::vec3& getScale() { return m_scale; }

	bool& usesNormalMap() { return m_useNormalMap; }
	bool& usesAlbedoMap() { return m_useAlbedoMap; }
	bool& usesRoughnessMap() { return m_useRoughnessMap; }
	bool& usesMetallicMap() { return m_useMetallicMap; }
	bool& usesAmbientOcclusionMap() { return m_useAmbientOcclusion; }


	bool& Wireframe() { return m_wireframe; }
	bool& backfaceCulling() { return m_backfaceCulling; }
	bool& cullMode() { return m_backfaceCullingmode; }
	bool& depthTesting() { return m_depthTesting; }
	bool& smooth() { return m_smoothShaded; }

	void setName(const std::string& name) { m_name = name; }
	std::string getName() const { return m_name; }

	void updateTransform();

	bool& visible() { return m_visible; }

	RT_ Material material;
private:
	GL_ Mesh* m_mesh;
	bool m_visible = true;
	bool m_useNormalMap = false;
	bool m_useAlbedoMap = false;
	bool m_useRoughnessMap = false;
	bool m_useMetallicMap = false;
	bool m_useAmbientOcclusion = false;
	glm::mat4 m_transform;
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_rotation = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	std::string m_name;
	bool m_wireframe = false;
	bool m_backfaceCulling = true;
	bool m_backfaceCullingmode = false; // false ^= clockwise; true ^= counterclockwise
	bool m_depthTesting = true;
	bool m_smoothShaded = false;
};


RT_END