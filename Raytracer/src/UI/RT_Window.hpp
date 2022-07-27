#pragma once

#include "../Core.hpp"
#include "../Log/RT_Log.hpp"
#include "GLFW/glfw3.h"
#include <functional>
#include <chrono>
RT_START

namespace ui
{

	struct WindowDimensions
	{
		float width;
		float height;
	};


	class Window
	{
	public:

		RT_Result create(std::string title, RT_ ui::WindowDimensions dimensions);
		void destroy();

		WindowDimensions getDimensions() { return m_size; }
		GLFWwindow* handle() { return m_window; }
		std::string getTitle() { return m_title; }

		void setKeyCallback(void (*f)(GLFWwindow* w, int key, int scancode, int action, int mods));
		void setCursorCallback(void (*f)(GLFWwindow* w, double xpos, double ypos));

		void startMainLoop(std::function<void(double)> f);
		
	private:
		GLFWwindow* m_window;
		std::string m_title;
		WindowDimensions m_size;
		bool m_allocated = false;
	};


}


RT_END