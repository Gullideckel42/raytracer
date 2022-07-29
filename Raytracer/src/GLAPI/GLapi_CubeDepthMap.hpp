#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <string>
#include "GLapi_GLerror.h"
#include "GLapi_Shader.hpp"
#include "GLapi_Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace h3dgl {
	class CubeDepthMap
	{
	public:
		void load(unsigned int size = 1024);

		GLuint map() { return m_depthMap; }

		void destroy();

		void bind(unsigned int slot);

		void unbind(unsigned int slot);

		bool loaded() { return m_loaded; }

	private:
		GLuint m_depthMap = 0;
		bool m_loaded = false;
	};
}
