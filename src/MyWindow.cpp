#include "MyWindow.h"
#include <stdexcept>

MyWindow::MyWindow(int width, int height, const char* title)
	: render(new Renderer()), windowSize(width, height), clearColor(1.0f, 1.0f, 1.0f, 1.0f)
{
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Failed to initialize GLEW");
	render->InitializeShader({
		{ GL_VERTEX_SHADER, "res/shaders/Vertex.shader" },
		{ GL_FRAGMENT_SHADER, "res/shaders/Fragment.shader" }
		});
}

MyWindow::~MyWindow()
{
	glfwTerminate();
}

void MyWindow::AddPicture(const std::string imagePath, uint32_t slot)
{
	pictures.push_back({ slot, imagePath });
	render->InitializeTextures(pictures);
}

void MyWindow::AddModels(Models model)
{
}

bool MyWindow::ShouldClose() const
{
	bool shouldClose = glfwWindowShouldClose(window);
	if (!shouldClose) {
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	return shouldClose;
}

void MyWindow::End() const
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void MyWindow::SetClearColor(float r, float g, float b, float a)
{
	clearColor = glm::vec4(r, g, b, a);
}
