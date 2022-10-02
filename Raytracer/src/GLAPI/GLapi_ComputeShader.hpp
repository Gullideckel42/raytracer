#pragma once
#include "GLapi_GLerror.h"
#include <fstream>
#include <sstream>
namespace h3dgl {

	class ComputeShader
	{
	private:
		GLuint m_program;
		GLuint m_computeshader;
		std::string m_path;
		static std::string ParseShader(const std::string& path);
	public:
		void load(const std::string& src);

		void reload();

		void destroy();
		void dispatch(unsigned int w, unsigned int h, unsigned int d, GLuint barrier = GL_ALL_BARRIER_BITS);
	};

}