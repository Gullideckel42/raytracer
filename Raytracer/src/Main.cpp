#include <iostream>
#include <chrono>

#ifdef RT_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "UI/RT_Window.hpp"
#include "GLAPI/GLapi.hpp"


// Global items
RT_START

RT_ ui::Window w;
GL_ Framebuffer fb; // The framebuffer the realtime renderer renders to

// The width of the render target (not the window size)
float r_width = 1920.0f;
float r_height = 1080.0f;

namespace Key
{
    bool R = false;
    bool T = false;
    bool Z = false;
    bool Space = false;
    bool ctrl = false;
    bool Mouse1 = false;
    bool Mouse2 = false;
    bool W = false, A = false, S = false, D = false;
    bool LEFT = false, RIGHT = false, UP = false, DOWN = false;
};

namespace Mouse
{
    glm::vec2 pos;
}

RT_END


#include "Renderer/RT_Renderer.hpp"
#include "Renderer/RT_Object.hpp"
#include "UI/GUI.hpp"

RT_START
void mainloop(double);


void key(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    bool c = false;

    if (action == GLFW_PRESS) c = true;
    else if (action == GLFW_RELEASE) c = false;
    else return;

    switch (key)
    {
    case GLFW_KEY_R:
        Key::R = c;
        break;
    case GLFW_KEY_T:
        Key::T = c;
        break;
    case GLFW_KEY_Y: // german keyboard layout
        Key::Z = c;
        break;
    case GLFW_KEY_SPACE:
        Key::Space = c;
        break;
    case GLFW_KEY_LEFT_CONTROL:
        Key::ctrl = c;
        break;
    case GLFW_KEY_W:
        Key::W = c;
        break;
    case GLFW_KEY_S:
        Key::S = c;
        break;
    case GLFW_KEY_A:
        Key::A = c;
        break;
    case GLFW_KEY_D:
        Key::D = c;
        break;
    case GLFW_KEY_LEFT:
        Key::LEFT = c;
        break;
    case GLFW_KEY_RIGHT:
        Key::RIGHT = c;
        break;
    case GLFW_KEY_UP:
        Key::UP = c;
        break;
    case GLFW_KEY_DOWN:
        Key::DOWN = c;
        break;
    }
}

void cursor(GLFWwindow* w, double xpos, double ypos)
{
    rt::Mouse::pos.x = xpos;
    rt::Mouse::pos.y = ypos;
    static bool firstMouse = true;
    static float mouseLastX, mouseLastY;
    static bool mouse5R = true;

    if (Key::Mouse2 && mouse5R)
    {
        mouse5R = false;
        firstMouse = true;
    }
    if (!Key::Mouse2) {
        mouse5R = true;
    }


    if (Key::Mouse1 && Key::Mouse2)
    {
        if (firstMouse)
        {
            mouseLastX = xpos;
            mouseLastY = ypos;
            firstMouse = false;
        }
        glm::vec3 viewDirection = renderer::c.lookAt() - renderer::c.position();
        float xoffset = xpos - mouseLastX;
        float yoffset = mouseLastY - ypos;
        mouseLastX = xpos;
        mouseLastY = ypos;
        renderer::c.lookAt() -= glm::normalize(glm::cross(viewDirection, renderer::c.up())) * xoffset * 0.02f;
        renderer::c.lookAt() -= glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * viewDirection) * yoffset * 0.02f;

    }
    else if (Key::Mouse2 && ui::sceneViewFocused) {
        if (firstMouse)
        {
            mouseLastX = xpos;
            mouseLastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - mouseLastX;
        float yoffset = mouseLastY - ypos;
        mouseLastX = xpos;
        mouseLastY = ypos;

        xoffset *= 0.32;
        yoffset *= 0.32;
        renderer::c.rotate(yoffset, xoffset);
    }
}

void mouse(GLFWwindow* w, int button, int action, int mods)
{
    bool c = false;

    if (action == GLFW_PRESS) c = true;
    else if (action == GLFW_RELEASE) c = false;
    else return;

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_2:
        Key::Mouse2 = c;
        break;
    case GLFW_MOUSE_BUTTON_1:
        Key::Mouse1 = c;
        break;
    }
}

void scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!ui::sceneViewFocused) return;
    renderer::c.radius() -= renderer::c.scrollSensitivity() * yoffset;
    if (renderer::c.radius() <= 0.1)  renderer::c.radius() = 0.1;
}

void init(RT_ ui::WindowDimensions windowSize, RT_ ui::WindowDimensions renderTargetSize)
{
    auto start = std::chrono::high_resolution_clock::now();
    r_width = renderTargetSize.width;
    r_height = renderTargetSize.height;

    rt_info("Core", "Begin initialization");
    CORE_ASSERT(w.create("Raytracer - OpenGl/C++ GLFW", { windowSize.width, windowSize.height }, key, cursor, mouse, scroll));

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
    fb.load(renderTargetSize.width, renderTargetSize.height, 1, attachments, false, GL_RGBA8, GL_UNSIGNED_BYTE);

    ui::init();
    ui::setDarkTheme();
    
    renderer::init(renderTargetSize.width, renderTargetSize.height);
    auto end = std::chrono::high_resolution_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    rt_info("Core", "Initialization completed within ", (float)elapsed / 1000.0, " seconds");
    rt_info("Core", "Started mainloop");
    w.startMainLoop(mainloop);
}

void update(double frametime)
{
    renderer::lastFrameMS = frametime;
    static bool r=true, t=true, z=true;

    if (renderer::c.following() != -1)
    {
        renderer::c.lookAt() = renderer::objects.at(renderer::c.following()).getPosition();
    }

    if (Key::R && r)
    {
        r = false;
        if (ui::selectedOperation == ui::GizmoOperation::ROTATE)
            ui::selectedOperation = ui::GizmoOperation::NONE;
        else
            ui::selectedOperation = ui::GizmoOperation::ROTATE;
    } 
    if (Key::T && t)
    {
        t = false;
        if (ui::selectedOperation == ui::GizmoOperation::TRANSLATE)
            ui::selectedOperation = ui::GizmoOperation::NONE;
        else
            ui::selectedOperation = ui::GizmoOperation::TRANSLATE;
    }
    if (Key::Z && z)
    {
        z = false;
        if (ui::selectedOperation == ui::GizmoOperation::SCALE)
            ui::selectedOperation = ui::GizmoOperation::NONE;
        else
            ui::selectedOperation = ui::GizmoOperation::SCALE;
    }


    if (Key::Space)
    {
        renderer::c.lookAt().y += renderer::c.movementspeed;
    }

    if (Key::ctrl)
    {
        renderer::c.lookAt().y -= renderer::c.movementspeed;
    }

    glm::vec3 viewDirection = renderer::c.lookAt() - renderer::c.position();

    if (Key::A)
    {
        renderer::c.lookAt() -= glm::normalize(glm::cross(viewDirection, renderer::c.up())) * renderer::c.movementspeed;
    }

    if (Key::D)
    {
        renderer::c.lookAt() += glm::normalize(glm::cross(viewDirection, renderer::c.up())) * renderer::c.movementspeed;
    }


    if (Key::W)
    {
        renderer::c.lookAt() += glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * viewDirection) * renderer::c.movementspeed;
    }

    if (Key::S)
    {
        renderer::c.lookAt() -= glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * viewDirection) * renderer::c.movementspeed;
    }


    if (Key::LEFT)
    {
        renderer::c.rotate(0, renderer::c.movementspeed);
    }
    if (Key::RIGHT)
    {
        renderer::c.rotate(0, -renderer::c.movementspeed);
    }
    if (Key::UP)
    {
        renderer::c.rotate(-renderer::c.movementspeed,0);
    }
    if (Key::DOWN)
    {
        renderer::c.rotate(renderer::c.movementspeed, 0);
    }

    if (!Key::R)
        r = true;
    if (!Key::T)
        t = true;
    if (!Key::Z)
        z = true;

}

void mainloop(double frametime)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    update(frametime);

    renderer::render();

    ui::docking(); // needs to be called first
    ui::sceneHierarchyPanel();
    ui::cameraPanel();
    ui::gBufferPanel();
    ui::propertiesPanel();
    ui::sceneView();
    ui::settingsPanel();
    ui::statisticsPanel();
    ui::meshLoaderPanel();

#ifdef RT_PLATFORM_WINDOWS
    Sleep(renderer::waitBetweenFramesMS);
#else
    std::this_thread::sleep_for(std::chrono::microseconds((long long) (renderer::waitBetweenFramesMS * 1000.0f)));
#endif

}

void dispose()
{
    renderer::dispose();
    rt_info("Core", "Disposed realtime renderer");
    ui::dispose();
    fb.destroy();
    w.destroy();
    rt_info("Core", "Disposed user interface and window");
}

RT_END

/*
 * Entry Point
 * 
 * 
 */

namespace rt
{
    int Main(int argc, char** argv)
    {
        int w = 1920;
        int h = 1080;
        if (argc == 3)
        {
            try
            {
                w = std::stoi(argv[1]);
                h = std::stoi(argv[2]);
            }
            catch (std::invalid_argument e) { rt_error("Initializer", "Entered invalid input data. It will revert to the default values (1920x1080)"); }
            catch (std::overflow_error e) { rt_error("Initializer", "Input data caused an overflow. It will revert to the default values (1920x1080)"); }
        }
        else if (argc == 1)
        {
            rt_warn("Initializer", "No command line input given. Renderer will boot up using default values (1920x1080)");
        }


        rt::init({ 1600, 900 }, { (float)w, (float)h });
        rt::dispose();
        rt_info("Initializer", "Terminated application");
        return 0;
    }
}

#if defined(RT_DEBUG) || defined(RT_RELEASE)

int main(int argc, char** argv)
{
    return rt::Main(argc, argv);
}

#elif defined(RT_DISTRIBUTION) && defined(RT_PLATFORM_WINDOWS)


// Run as windows application in distribution mode

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return rt::Main(__argc, __argv);
}

#endif