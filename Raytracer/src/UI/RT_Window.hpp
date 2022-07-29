#pragma once

#include "../Core.hpp"

#include "GLFW/glfw3.h"
#include <functional>
#include <chrono>

#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_impl_glfw.h"
#include "../imgui-docking/imgui_impl_opengl3.h"

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

		RT_Result create(std::string title, RT_ ui::WindowDimensions dimensions,
			void (*keyCallback)(GLFWwindow* w, int key, int scancode, int action, int mods),
			void (*cursorCallback)(GLFWwindow* w, double xpos, double ypos),
			void (*mouseCallback)(GLFWwindow* w, int button, int action, int mods),
			void (*scrollCallback) (GLFWwindow* window, double xoffset, double yoffset)
			);
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