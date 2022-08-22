#pragma once
#include "../Core.hpp"
#include "../GLAPI/GLapi.hpp"

RT_START

class Camera
{
public:

	void create(float fov, float width, float height, float zNear, float zFar);

	void rotate(float xRel, float yRel);

	void update();


	glm::mat4& view() { return m_view; }
	glm::mat4& projection() { return m_projection; }
	glm::mat4& viewProjection() { return m_viewProj; }
	glm::vec3& position() { return m_position; }

	glm::vec3& lookAt() { return m_lookAt; }
	glm::vec2& rotation() { return m_rotation;}

	float& radius() { return r; }

	float& fov() { return m_fov; }
	float& nearPlane() { return m_near; }
	float& farPlane() { return m_far; }
	float& width() { return m_width; }
	float& height() { return m_height; }

	inline glm::vec3& up() { return m_up; }

	float& scrollSensitivity() { return m_scrollSensitivity; }

	void updateProj();

	int& following() { return m_following; }

	float movementspeed = 0.2f;
private:

	float m_mouseSensitivityX = 0.05;
	float m_mouseSensitivityY = 0.05;

	float m_fov; // Degree
	float m_near;
	float m_far;
	float m_width;
	float m_height;

	int m_following = -1;

	float r = 3.0f;
	glm::vec2 m_rotation = { 0, 0 };
	glm::vec3 m_lookAt = {0,0,-6};
	glm::vec3 m_up = {0, 1, 0};
	
	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_viewProj;
	glm::vec3 m_position;

	float m_scrollSensitivity = 0.5;
};


RT_END