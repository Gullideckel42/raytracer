#include "GLapi_ComputeShader.hpp"

void h3dgl::ComputeShader::load(const std::string& path) {
	GLCALL(m_computeshader = glCreateShader(GL_COMPUTE_SHADER));
	std::string src = ParseShader(path);
	const char* src_cstr = src.c_str();
	GLCALL(glShaderSource(m_computeshader, 1, &src_cstr, NULL));
	GLCALL(glCompileShader(m_computeshader));
	int result;
	GLCALL(glGetShaderiv(m_computeshader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCALL(glGetShaderiv(m_computeshader, GL_INFO_LOG_LENGTH, &length));
		char* message = new char[length];

		GLCALL(glGetShaderInfoLog(m_computeshader, length, &length, message));
		rt_error("GL", "Failed to compile compute shader: ", "\n", message);
		delete[] message;
		GLCALL(glDeleteShader(m_computeshader));
		return;
	}
	GLCALL(m_program = glCreateProgram());
	GLCALL(glAttachShader(m_program, m_computeshader));
	GLCALL(glLinkProgram(m_program));
	rt_info("GL", "Loaded compute shader");
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
	rt_info("GL", "Disposed compute shader");
}

void h3dgl::ComputeShader::dispatch(unsigned int w, unsigned int h, unsigned int d, GLuint barrier) {
	GLCALL(glUseProgram(m_program));
	GLCALL(glDispatchCompute(w, h, d));
	GLCALL(glMemoryBarrier(barrier));
	GLCALL(glUseProgram(0));
}