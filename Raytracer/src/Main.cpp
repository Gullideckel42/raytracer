#include <iostream>
#include <chrono>

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
    bool L = false;
    bool Mouse5 = false;
};


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
    case GLFW_KEY_L:
        Key::L = c;
    default:
        break;
    }
}

void cursor(GLFWwindow* w, double xpos, double ypos)
{
    static bool firstMouse = true;
    static float mouseLastX, mouseLastY;
    static bool mouse5R = true;

    if (Key::Mouse5 && mouse5R)
    {
        mouse5R = false;
        firstMouse = true;
    }
    if (!Key::Mouse5) {
        mouse5R = true;
    }

    if (Key::Mouse5 && ui::sceneViewFocused) {
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
        Key::Mouse5 = c;
    default:
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
    if (Key::L)
    {
        
    }


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
    std::this_thread::sleep_for(std::chrono::microseconds((long long) renderer::waitBetweenFramesMS * 1000));
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
int main(int argc, char** argv)
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


    rt::init({ 1600, 900 }, {(float) w, (float) h});
    rt::dispose();
    rt_info("Initializer", "Terminated application");

 	return 0;
}