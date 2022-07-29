#include "RT_Object.hpp"

bool RT_ Object::create(std::string name, GL_ Mesh* mesh, RT_ Material mat)
{
    
    m_name = name;
    m_mesh = mesh;
    material = mat;

    m_position = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f);
    m_rotation = glm::vec3(0.0f);


    rt_info("Created new object: ", name);

    return true;
}

void RT_ Object::bind()
{
    m_mesh->bind();

    if (m_useAlbedoMap && material.albedo != nullptr)
        material.albedo->bind(0);
    if (m_useNormalMap && material.normal != nullptr)
        material.normal->bind(1);
    if (m_useRoughnessMap && material.roughness != nullptr)
        material.roughness->bind(2);
    if (m_useMetallicMap && material.metallic != nullptr)
        material.metallic->bind(3);
    if (m_useAmbientOcclusion && material.ambientOcclusion != nullptr)
        material.ambientOcclusion->bind(4);

}

void RT_ Object::unbind()
{
    if (m_useAlbedoMap && material.albedo != nullptr)
        material.albedo->unbind(0);
    if (m_useNormalMap && material.normal != nullptr)
        material.normal->unbind(1);
    if (m_useRoughnessMap && material.roughness != nullptr)
        material.roughness->unbind(2);
    if (m_useMetallicMap && material.metallic != nullptr)
        material.metallic->unbind(3);
    if (m_useAmbientOcclusion && material.ambientOcclusion != nullptr)
        material.ambientOcclusion->unbind(4);
    m_mesh->unbind();
}

void RT_ Object::updateTransform()
{
    m_transform = glm::mat4(1.0f);
    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::rotate(m_transform, m_rotation.x, { 1,0,0 });
    m_transform = glm::rotate(m_transform, m_rotation.y, { 0,1,0 });
    m_transform = glm::rotate(m_transform, m_rotation.z, { 0,0,1 });
    m_transform = glm::scale(m_transform, m_scale);
}
