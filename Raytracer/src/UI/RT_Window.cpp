#include "RT_Window.hpp"


bool RT_ ui::Window::create(std::string title, RT_ ui::WindowDimensions dimensions,
	void (*keyCallback)(GLFWwindow* w, int key, int scancode, int action, int mods),
	void (*cursorCallback)(GLFWwindow* w, double xpos, double ypos),
	void (*mouseCallback)(GLFWwindow* w, int button, int action, int mods),
	void (*scrollCallback) (GLFWwindow* window, double xoffset, double yoffset)
	)
{
	if (m_allocated) return false;


	if (!glfwInit())
	{
		rt_fatal("GLFW Initialization failed");
		return false;
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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

	setKeyCallback(keyCallback);
	setCursorCallback(cursorCallback);
	glfwSetMouseButtonCallback(m_window, mouseCallback);
	glfwSetScrollCallback(m_window, scrollCallback);

	m_size.width = dimensions.width;
	m_size.height = dimensions.height;
	m_title = title;

	// Init Dear ImGui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& m_imguiIO = ImGui::GetIO();
	m_imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifdef RT_PLATFORM_WINDOWS
	m_imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#else
#error Windows!!!
#endif

	m_imguiIO.FontDefault = m_imguiIO.Fonts->AddFontFromFileTTF("assets/font/OpenSans/OpenSans-ExtraBold.ttf", 15.5f);


	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");


	m_allocated = true;
	rt_info("Created window(", m_size.width, "x", m_size.height, ") successful");
	return true;
}

void RT_ ui::Window::destroy()
{
	if (!m_allocated) return;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		f(frametime);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGuiIO& io = ImGui::GetIO();
#ifdef RT_PLATFORM_WINDOWS
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)2));
		auto end = std::chrono::high_resolution_clock::now();
		long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		frametime = (double)elapsed;
	}
}