#include "GLapi_CubeDepthMap.hpp"


void h3dgl::CubeDepthMap::load(unsigned int size)
{
	GLCALL(glGenTextures(1, &m_depthMap));

	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap));

	for (int i = 0; i < 6; i++)
	{
		GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
	}

	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	m_loaded = true;
}

void h3dgl::CubeDepthMap::destroy() {
	m_loaded = false;
	GLCALL(glDeleteTextures(1, &m_depthMap));
}


void h3dgl::CubeDepthMap::bind(unsigned int slot)
{
	GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap));
}

void h3dgl::CubeDepthMap::unbind(unsigned int slot)
{
	GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
	GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap));
}