#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>

#include "GLapi_GLerror.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace h3dgl {
    class Shader {
    private:
        GLuint id;
        mutable std::unordered_map<std::string, GLint> m_uniformLocationsCache;
        unsigned int CompileShader(GLuint type, const std::string& source);
        bool hasGeometryShader;
        unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);

        struct ShaderProgramSource {
            std::string VertexSource;
            std::string FragmentSource;
            std::string geometryShaderSource;
        };

        ShaderProgramSource ParseShader(const std::string& path);

        GLint getUniformLocation(const std::string& name) const;

    public:

        void create(const std::string& path, bool geometryShader=false);

        void destroy();

        GLuint Id();

        void use();

        void unuse();

        void setUniform1i(const std::string& name, int i);

        void setUniform1f(const std::string& name, float f);

        void setUniform2f(const std::string& name, float f0, float f1);

        void setUniform3f(const std::string& name, float f0, float f1, float f2);

        void setUniform4f(const std::string& name, float f0, float f1, float f2, float f3);

        void setUniform1d(const std::string& name, double d);

        void setVector2f(const std::string& name, const glm::vec2& v0);

        void setVector3f(const std::string& name, const glm::vec3& v0);

        void setVector4f(const std::string& name, const glm::vec4& v0);

        void setMatrix2f(const std::string& name, const glm::mat2& m);

        void setMatrix3f(const std::string& name, const glm::mat3& m);


        void setMatrix4f(const std::string& name, const glm::mat4& m);

    };
}