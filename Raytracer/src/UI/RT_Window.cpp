#include "RT_Window.hpp"

bool RT_ ui::Window::create(std::string title, RT_ ui::WindowDimensions dimensions)
{
	if (m_allocated) return false;


	if (!glfwInit())
	{
		rt_fatal("GLFW Initialization failed");
		return false;
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(dimensions.width, dimensions.height, title.data(), nullptr, nullptr);

	if (!m_window)
	{
		rt_fatal("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (glewInit() != GLEW_OK)
	{
		rt_fatal("GLEW Initialization failed");
		glfwTerminate();
		return false;
	}

	rt_info("OpenGL API Version: ", glGetString(GL_VERSION));


	m_size.width = dimensions.width;
	m_size.height = dimensions.height;
	m_title = title;

	m_allocated = true;
	rt_info("Created window(", m_size.width, "x", m_size.height, ") successful");
	return true;
}

void RT_ ui::Window::destroy()
{
	if (!m_allocated) return;

	if(m_window)
	glfwDestroyWindow(m_window);
	m_window = nullptr;
	glfwTerminate();
	m_allocated = false;
	rt_info("Destroyed window(", m_size.width, "x", m_size.height, ") successful")
}


void RT_ ui::Window::setKeyCallback(void (*f)(GLFWwindow* w, int key, int scancode, int action, int mods))
{
	glfwSetKeyCallback(m_window, f);
}

void RT_ ui::Window::setCursorCallback(void (*f)(GLFWwindow* w, double xpos, double ypos))
{
	glfwSetCursorPosCallback(m_window, f);
}

void RT_ ui::Window::startMainLoop(std::function<void(double)> f)
{
	double frametime = 0.0f;
	while (!glfwWindowShouldClose(m_window))
	{
		auto start = std::chrono::high_resolution_clock::now();
		f(frametime);
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		auto end = std::chrono::high_resolution_clock::now();
		long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		frametime = (double)elapsed;
	}
}