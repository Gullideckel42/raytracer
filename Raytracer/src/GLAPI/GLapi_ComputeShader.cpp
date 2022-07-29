#include "GLapi_ComputeShader.hpp"

void h3dgl::ComputeShader::load(const std::string& path) {
	GLCALL(m_computeshader = glCreateShader(GL_COMPUTE_SHADER));
	std::string src = ParseShader(path);
	const char* src_cstr = src.c_str();
	GLCALL(glShaderSource(m_computeshader, 1, &src_cstr, NULL));
	GLCALL(glCompileShader(m_computeshader));
	GLCALL(m_program = glCreateProgram());
	GLCALL(glAttachShader(m_program, m_computeshader));
	GLCALL(glLinkProgram(m_program));
}

std::string h3dgl::ComputeShader::ParseShader(const std::string& path) {
	std::ifstream stream(path);
	std::string line;
	std::stringstream sstream;
	sstream << "";
	while (getline(stream, line)) {
		sstream << line << "\n";
	}
	stream.close();
	return sstream.str();
}

void h3dgl::ComputeShader::destroy() {
	GLCALL(glDeleteShader(m_computeshader));
	GLCALL(glDeleteProgram(m_program));
}

void h3dgl::ComputeShader::run(unsigned int w, unsigned int h, unsigned int d) {
	glUseProgram(m_program);
	glDispatchCompute(w, h, d);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glUseProgram(0);
}