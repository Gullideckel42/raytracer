#include <iostream>


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
        renderer::c.rotate(yoffset, -xoffset);
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
    renderer::c.radius() -= 0.1*yoffset;
    if (renderer::c.radius() <= 0.1)  renderer::c.radius() = 0.1;
}

void init(float width, float height)
{
    rt_info("Begin initialization!");
    CORE_ASSERT(w.create("Raytracer - OpenGl/C++ GLFW", { width, height }, key, cursor, mouse, scroll));
   

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
    fb.load(r_width, r_height, 1, attachments, false);

    ui::setDarkTheme();

    renderer::init(r_width, r_height);

    rt_info("Started mainloop");
    w.startMainLoop(mainloop);
}

void update(double frametime)
{

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

}

void dispose()
{
    renderer::dispose();
    fb.destroy();
    w.destroy();
}

RT_END

/*
 * Entry Point
 * 
 * 
 */
int main(int argc, char** argv)
{
    

    rt::init(1600, 900);
    rt::dispose();


 	return 0;
}