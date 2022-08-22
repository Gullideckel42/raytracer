#include "GLapi_Shader.hpp"


unsigned int h3dgl::Shader::CompileShader(GLuint type, const std::string& source)
{
    GLCALL(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCALL(glShaderSource(id, 1, &src, nullptr));
    GLCALL(glCompileShader(id));
    int result;
    GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];

        GLCALL(glGetShaderInfoLog(id, length, &length, message));
        rt_error("GL", message);
        delete[] message;
        GLCALL(glDeleteShader(id));
        return 0;
    }
    return id;
}

unsigned int h3dgl::Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
    GLCALL(GLuint program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    unsigned int gs = 0;
    if (hasGeometryShader)
        gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    GLCALL(glAttachShader(program, vs));
    GLCALL(glAttachShader(program, fs));
    if (hasGeometryShader) {
        GLCALL(glAttachShader(program, gs));
    }
    GLCALL(glLinkProgram(program));
    GLCALL(glValidateProgram(program));
    GLCALL(glDeleteShader(vs));
    GLCALL(glDeleteShader(fs));
    if (hasGeometryShader) {
        GLCALL(glDeleteShader(gs));
    }

    return program;
}

h3dgl::Shader::ShaderProgramSource h3dgl::Shader::ParseShader(const std::string& path) {
    std::ifstream stream(path);
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 };
    ShaderType type = ShaderType::NONE;
    std::string line;
    std::stringstream sStream[3];
    sStream[0] << "";
    sStream[1] << "";
    sStream[2] << "";
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
            else if (line.find("geometry") != std::string::npos && hasGeometryShader)
            {
                type = ShaderType::GEOMETRY;
            }
        }
        else
        {
            if (type != ShaderType::NONE)
                sStream[(int)type] << line << "\n";
        }
    }
    return { sStream[0].str(), sStream[1].str(), sStream[2].str() };
}

GLint h3dgl::Shader::getUniformLocation(const std::string& name) const {
    if (m_uniformLocationsCache.find(name) != m_uniformLocationsCache.end()) {
        return m_uniformLocationsCache[name];
    }
    GLCALL(GLint location = glGetUniformLocation(id, name.c_str()));
    m_uniformLocationsCache[name] = location;
    return location;
}

void h3dgl::Shader::create(const std::string& path, bool geometryShader) {
    hasGeometryShader = geometryShader;
    m_path = path;
    h3dgl::Shader::ShaderProgramSource source = ParseShader(path);
    id = CreateShader(source.VertexSource, source.FragmentSource, source.geometryShaderSource);
    GLCALL(glUseProgram(0));
    rt_info("GL", "Loaded shader: ", m_path);
}

void h3dgl::Shader::reload()
{
    destroy();
    h3dgl::Shader::ShaderProgramSource source = ParseShader(m_path);
    id = CreateShader(source.VertexSource, source.FragmentSource, source.geometryShaderSource);
    m_uniformLocationsCache.clear();
    GLCALL(glUseProgram(0));
    rt_info("GL", "Reloaded shader: ", m_path);
}

void h3dgl::Shader::destroy() {
    GLCALL(glDeleteProgram(id));
    rt_info("GL", "Deleted shader program: ", m_path);
}

GLuint h3dgl::Shader::Id() {
    return id;
}

void h3dgl::Shader::use() {
    GLCALL(glUseProgram(id));
}

void h3dgl::Shader::unuse() {
    GLCALL(glUseProgram(0));
}

void h3dgl::Shader::setUniform1i(const std::string& name, int i) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform1i(uniformLocation, i));
}

void h3dgl::Shader::setUniform1f(const std::string& name, float f) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform1f(uniformLocation, f));
}

void h3dgl::Shader::setUniform2f(const std::string& name, float f0, float f1) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform2f(uniformLocation,f0, f1));
}

void h3dgl::Shader::setUniform3f(const std::string& name, float f0, float f1, float f2) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform3f(uniformLocation, f0, f1, f2));
}

void h3dgl::Shader::setUniform4f(const std::string& name, float f0, float f1, float f2, float f3) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform4f(uniformLocation, f0, f1, f2, f3));
}

void h3dgl::Shader::setUniform1d(const std::string& name, double d) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform1d(uniformLocation, d));
}

void h3dgl::Shader::setVector2f(const std::string& name, const glm::vec2& v0)
{
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform2f(uniformLocation, v0.x, v0.y));
}

void h3dgl::Shader::setVector3f(const std::string& name, const glm::vec3& v0)
{
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform3f(uniformLocation, v0.x, v0.y, v0.z));
}

void h3dgl::Shader::setVector4f(const std::string& name, const glm::vec4& v0)
{
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniform4f(uniformLocation, v0.x, v0.y, v0.z, v0.w));
}


void h3dgl::Shader::setMatrix2f(const std::string& name, const glm::mat2& m) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, &m[0][0]));
}

void h3dgl::Shader::setMatrix3f(const std::string& name, const glm::mat3& m) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &m[0][0]));
}

void h3dgl::Shader::setMatrix4f(const std::string& name, const glm::mat4& m) {
    GLint uniformLocation = getUniformLocation(name);
    GLCALL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m[0][0]));
}