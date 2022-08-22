#include "GLapi_EquirectangularMap.h"
#include "ThirdParty/stb/stb_image.h"
// Equirectangular 

void h3dgl::EquirectangularMap::load(const std::string& path) {
    m_path = path;
    stbi_set_flip_vertically_on_load(true);
    GLCALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
    unsigned int hdrTexture;
    float* data = stbi_loadf(path.c_str(), &m_width, &m_height, &m_bitsPerPixel, 0);
    if (data)
    {
        GLCALL(glGenTextures(1, &hdrTexture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, hdrTexture));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, data));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        //GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
        stbi_image_free(data);
    }
    else
    {
        rt_error("GL", "Failed to load cubemap");
        return;
    }
    // Convert to cubemap

    h3dgl::Mesh cube;
    h3dgl::Vertex cubeVertex[] = H3D_CUBE_VERTICES(1.0, 1.0, 1.0f);
    uint32_t cubeIndex[] = H3D_CUBE_INDICES;
    cube.load(cubeVertex, 8, cubeIndex, 36);

    h3dgl::Shader convertionShader;
    convertionShader.create("src/shader/EquiToCube.shader", false);

    unsigned int captureFBO, captureRBO;
    GLCALL(glGenFramebuffers(1, &captureFBO));
    GLCALL(glGenRenderbuffers(1, &captureRBO));

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048));
    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO));

    GLCALL(glGenTextures(1, &m_cubemap));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap));
    
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
   
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    convertionShader.use();
    cube.bind();
    convertionShader.setUniform1i("equirectangularMap", 0);
    convertionShader.setMatrix4f("projection", captureProjection);
    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, hdrTexture));
    GLCALL(glDisable(GL_CULL_FACE));
    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glViewport(0, 0, 2048, 2048));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    for (unsigned int i = 0; i < 6; ++i)
    {
        convertionShader.setMatrix4f("view", captureViews[i]);
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemap, 0));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCALL(glDrawElements(GL_TRIANGLES, cube.indexCount(), GL_UNSIGNED_INT, NULL));
    }
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    cube.unbind();
    convertionShader.unuse();
    GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    // Delete
    GLCALL(glDeleteTextures(1, &hdrTexture));
    GLCALL(glDeleteFramebuffers(1, &captureFBO));
    GLCALL(glDeleteRenderbuffers(1, &captureRBO));
    convertionShader.destroy();
    cube.destroy();
    m_loaded = true;
    rt_info("GL", "Loaded and transformed environment map into cubemap format");
}

bool h3dgl::EquirectangularMap::isLoaded() {
    return m_loaded;
}

void h3dgl::EquirectangularMap::destroy() {
    if (!m_loaded) return;
    GLCALL(glDeleteTextures(1, &m_cubemap));
    m_loaded = false;
    rt_info("GL", "Destroyed cubemap");
}

void h3dgl::EquirectangularMap::bind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap));
}

void h3dgl::EquirectangularMap::unbind(unsigned int slot) const {
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

int h3dgl::EquirectangularMap::getWidth() const {
    return m_width;
}

int h3dgl::EquirectangularMap::getHeight() const {
    return m_height;
}

GLuint h3dgl::EquirectangularMap::getId() {
    return m_cubemap;
}

// Not intended for realtime
h3dgl::EquirectangularMap h3dgl::EquirectangularMap::convolute() const {
    auto t0 = std::chrono::high_resolution_clock::now();

    h3dgl::Shader convolutionShader;
    convolutionShader.create("src/shader/Convolution.shader");
    h3dgl::Mesh cube;
    h3dgl::Vertex vertices[] = H3D_CUBE_VERTICES(1.0, 1.0, 1.0);
    uint32_t indices[] = H3D_CUBE_INDICES;
    cube.load(vertices, 8, indices, 36);

    GLuint irradianceMap = 0;

    GLCALL(glGenTextures(1, &irradianceMap));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
    for (unsigned int i = 0; i < 6; ++i)
    {
        GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 64, 64, 0,
            GL_RGB, GL_FLOAT, nullptr));
    }
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    unsigned int captureFBO, captureRBO;
    GLCALL(GLCALL(glGenFramebuffers(1, &captureFBO)));
    GLCALL(GLCALL(glGenRenderbuffers(1, &captureRBO)));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64));

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    convolutionShader.use();
    convolutionShader.setUniform1i("environmentMap", 0);
    convolutionShader.setMatrix4f("projection", captureProjection);
    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap));
    GLCALL(glDisable(GL_CULL_FACE));
    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glViewport(0, 0, 64, 64)); // don't forget to configure the viewport to the capture dimensions.
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    cube.bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        convolutionShader.setMatrix4f("view", captureViews[i]);
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        GLCALL(glDrawElements(GL_TRIANGLES, cube.indexCount(), GL_UNSIGNED_INT, nullptr));

    }
    cube.unbind();
    convolutionShader.unuse();
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(glDeleteRenderbuffers(1, &captureRBO));
    GLCALL(glDeleteFramebuffers(1, &captureFBO));
    cube.destroy();
    convolutionShader.destroy();

    h3dgl::EquirectangularMap map;
    map.m_cubemap = irradianceMap;
    map.m_height = 64;
    map.m_width = 64;
    map.m_loaded = true;
    map.m_path = "";
    map.m_bitsPerPixel = 3 * 8;
    auto t1 = std::chrono::high_resolution_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    rt_info("GL", "Convoluted cubemap to diffuse iradiance map (Took ", ((double)elapsed / 1000.0), " seconds)");
    return map;
}

h3dgl::EquirectangularMap h3dgl::EquirectangularMap::prefilter() const {
    
    auto t0 = std::chrono::high_resolution_clock::now();

    h3dgl::Shader prefilterShader;
    prefilterShader.create("src/shader/Prefilter.shader");
    h3dgl::Mesh cube;
    h3dgl::Vertex vertices[] = H3D_CUBE_VERTICES(1.0, 1.0, 1.0);
    uint32_t indices[] = H3D_CUBE_INDICES;
    cube.load(vertices, 8, indices, 36);
    constexpr int size = 2048;
    GLuint prefilterMap = 0;
    GLCALL(glGenTextures(1, &prefilterMap));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap));
    for (unsigned int i = 0; i < 6; ++i)
    {
        GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0,
            GL_RGB, GL_FLOAT, nullptr));
    }
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

    unsigned int captureFBO, captureRBO;
    GLCALL(GLCALL(glGenFramebuffers(1, &captureFBO)));
    GLCALL(GLCALL(glGenRenderbuffers(1, &captureRBO)));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size));

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    prefilterShader.use();
    prefilterShader.setUniform1i("environmentMap", 0);
    prefilterShader.setMatrix4f("projection", captureProjection);
    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap));
    GLCALL(glDisable(GL_CULL_FACE));
    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    cube.bind();
    constexpr int maxMipLevels = 7;

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipWidth = size * std::pow(0.5, mip);
        unsigned int mipHeight = size * std::pow(0.5, mip);
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight));
        GLCALL(glViewport(0, 0, mipWidth, mipHeight));
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setUniform1f("roughness", roughness);
        for (int i = 0; i < 6; i++) {
            prefilterShader.setMatrix4f("view", captureViews[i]);
            GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip));
            GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            GLCALL(glDrawElements(GL_TRIANGLES, cube.indexCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    cube.unbind();
    prefilterShader.unuse();
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(glDeleteRenderbuffers(1, &captureRBO));
    GLCALL(glDeleteFramebuffers(1, &captureFBO));
    cube.destroy();
    prefilterShader.destroy();

    h3dgl::EquirectangularMap map;
    map.m_cubemap = prefilterMap;
    map.m_height = size;
    map.m_width = size;
    map.m_loaded = true;
    map.m_path = "";
    map.m_bitsPerPixel = 3 * 8;
    
    auto t1 = std::chrono::high_resolution_clock::now();

    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    rt_info("GL", "Prefiltered cubemap (Took ", ((double)elapsed / 1000.0), " seconds)");
    return map;
}



void h3dgl::EquirectangularMap::load(
    const std::string& pX,
    const std::string& pY,
    const std::string& pZ,
    const std::string& nX,
    const std::string& nY,
    const std::string& nZ
)
{
    GLCALL(glGenTextures(1, &m_cubemap));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap));

    std::string files[] = { pX, nX, pY, nY, pZ, nZ };
    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < 6; i++) {

        auto texBuffer = stbi_load(files[i].c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);
        GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, m_width, m_height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer));
        if (texBuffer) {
            stbi_image_free(texBuffer);
        }
        else {
            std::cout << "Texture could not be loaded: Cubemap" << std::endl;
        }
    }

    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    m_loaded = true;
}