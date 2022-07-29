#ifndef H3DGLAPI_GLERROR

    #define H3DGLAPI_GLERROR
    #define GLEW_STATIC
    #include <GL/glew.h>
    #include <string>
    #include <iostream>
#include "../Core.hpp"
#include "../Log/RT_Log.hpp"

    #define GLapi_OPENGLCALLS_DEBUG_ERROR_CHECK
    #ifdef GLapi_OPENGLCALLS_DEBUG_ERROR_CHECK

    #define ASSERT(x) if (!(x)) __debugbreak()

    #define GLCALL(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

    static void GLClearError()
    {
        while (glGetError() != GL_NO_ERROR);
    }

    static std::string GetGLErrorString(GLenum errorcode) {
        switch (errorcode)
        {
        case GL_INVALID_ENUM:
            return std::string("GL_INVALID_ENUM");
        case GL_INVALID_VALUE:
            return std::string("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION:
            return std::string("GL_INVALID_OPERATION");
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return std::string("GL_INVALID_FRAMEBUFFER_OPERATION");
        case GL_OUT_OF_MEMORY:
            return std::string("GL_OUT_OF_MEMORY");
        case GL_STACK_UNDERFLOW:
            return std::string("GL_STACK_UNDERFLOW");
        case GL_STACK_OVERFLOW:
            return std::string("GL_STACK_OVERFLOW");
        default:
            return std::string("UNKNOWN_ERROR");
        }
    }

    static bool GLLogCall(const char* function, const char* file, int line)
    {
        bool errorTriggered = false;
        uint8_t errors = 0;
        constexpr const char* red = "\033[91m";
        constexpr const char* blue = "\x1b[36m";
        constexpr const char* reset = "\x1B[0m";
        constexpr const char* yellow = "\033[93m";

        while (GLenum error = glGetError()) {

            rt_error("{OpenGL Error 0x", std::hex, error, std::dec, "}: ", blue, function, reset, " at ", file, ":", yellow, line, reset,
                " -- ", red, GetGLErrorString(error), reset);

            //std::cout << red << "[OpenGL Error]" << reset << " (Code: 0x" << std::hex << error << std::dec << "; " << error << ") : "
            //    << blue << function << reset << " at " << file << ":" << yellow << line << reset << " -- "
            //    << red << GetGLErrorString(error) << reset << std::endl;
            errorTriggered = true;
            errors++;
        }
        if (errorTriggered) std::cout << std::endl << "Number of errors occurred: " << red << (int)errors << reset << std::endl
            << "See " << blue << "https://docs.gl/" << reset << " for detailed information" << std::endl;
        return !errorTriggered;
    }

    #else
    #define GLCALL(x) x
    #define ASSERT(x) x
    #endif

#endif