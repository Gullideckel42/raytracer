#include <iostream>


#include "Log/RT_Log.hpp"
#include "UI/RT_Window.hpp"

RT_START

RT_ ui::Window w;

void mainloop(double);

void init(float width, float height)
{
    CORE_ASSERT(w.create("Title", { width, height }));
    w.startMainLoop(mainloop);
}

void mainloop(double frametime)
{
    
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
int main()
{

    rt::init(1280, 720);
    rt::dispose();


 	return 0;
}