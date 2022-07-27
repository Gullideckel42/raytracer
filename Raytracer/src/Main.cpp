#include <iostream>

#include "UI/GUI.hpp"

#include "UI/RT_Window.hpp"


RT_START

RT_ ui::Window w;

void mainloop(double);

void init(float width, float height)
{
    CORE_ASSERT(w.create("Title", { width, height }));
    ui::setDarkTheme();
    w.startMainLoop(mainloop);
}

void mainloop(double frametime)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    ui::propertiesPanel();
    ui::sceneView();
}

void dispose()
{
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