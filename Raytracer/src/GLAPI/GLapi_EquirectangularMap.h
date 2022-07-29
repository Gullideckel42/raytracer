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

namespace h3dgl {
	class EquirectangularMap {
	private:
		GLuint m_cubemap;
		int m_width = 0;
		int m_height = 0;
		int m_bitsPerPixel = 0;
		bool m_loaded = false;
		std::string m_path = "";
	public:
		void load(const std::string& path);

		void load(
			const std::string& pX,
			const std::string& pY,
			const std::string& pZ,
			const std::string& nX,
			const std::string& nY,
			const std::string& nZ
		);

		bool isLoaded();

		void destroy();

		void bind(unsigned int slot) const;

		void unbind(unsigned int slot) const;

		int getWidth() const;

		int getHeight() const;

		std::string path() { return m_path; }

		h3dgl::EquirectangularMap convolute() const;

		h3dgl::EquirectangularMap prefilter() const;

		GLuint getId();
	};
}