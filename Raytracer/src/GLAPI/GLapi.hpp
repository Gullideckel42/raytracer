#pragma once
// ------------------------ C++ Standard Headers ------------------------ //
#include <iostream>
#include <string>
#include <vector>
// ----------------------------- Third Pary ----------------------------- //
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GL_ ::h3dgl::

// ----------------------------- OpenGL API ----------------------------- //
#include "GLapi_GLerror.h"
#include "GLapi_Vertex.hpp"
#include "GLapi_Framebuffer.hpp"
#include "GLapi_VertexBuffer.hpp"
#include "GLapi_Shader.hpp"
#include "GLapi_Mesh.hpp"
#include "GLapi_Texture.hpp"
#include "GLapi_EquirectangularMap.h"
#include "GLapi_CubeDepthMap.hpp"
#include "GLapi_IndexBuffer.hpp"
#include "GLapi_ComputeShader.hpp"