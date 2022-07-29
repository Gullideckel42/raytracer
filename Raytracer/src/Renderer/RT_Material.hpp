#pragma once
#include "../Core.hpp"
#include "../GLAPI/GLapi.hpp"

RT_START

struct Material
{
	glm::vec3 _albedo;
	float _roughness;
	float _metallic;

	GL_ Texture* albedo = nullptr;
	GL_ Texture* normal = nullptr;
	GL_ Texture* roughness = nullptr;
	GL_ Texture* metallic = nullptr;
	GL_ Texture* ambientOcclusion = nullptr;
};

RT_END