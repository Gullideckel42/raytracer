#include "RT_Camera.hpp"

void RT_ Camera::create(float fov, float width, float height, float zNear, float zFar)
{
	m_fov = fov;
	m_width = width;
	m_height = height;
	m_near = zNear;
	m_far = zFar;
	m_view = glm::mat4(1.0f);
	m_projection = glm::perspective(glm::radians(fov)/2.0f, width / height, zNear, zFar);
	m_position = glm::vec3(0.0f);
	m_viewProj = m_projection * m_view;
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_view = glm::lookAt(m_position, m_position + m_lookAt, m_up);
    update();
}

void RT_ Camera::update()
{
	m_up = { 0,-1,0 };

	
	m_position = m_lookAt;
	m_position.x += r * abs(sin(m_rotation.x)) * cos(m_rotation.y);
	m_position.z += r * abs(sin(m_rotation.x)) * sin(m_rotation.y);
	m_position.y += r * cos(m_rotation.x);

    m_view = glm::lookAt(m_position, m_lookAt, m_up);
	m_viewProj = m_projection * m_view;
}

void RT_ Camera::rotate(float xRel, float yRel) {
	

	m_rotation.x += xRel * m_mouseSensitivityX;
	m_rotation.y += yRel * m_mouseSensitivityY;
	
	if (m_rotation.x >= 3.14159) m_rotation.x = 3.14159;
	if (m_rotation.x <= 0.01) m_rotation.x = 0.01;

    update();
}


void RT_ Camera::updateProj()
{

	m_projection = glm::perspective(glm::radians(m_fov) / 2.0f, m_width / m_height, m_near, m_far);
	m_viewProj = m_projection * m_view;
}
