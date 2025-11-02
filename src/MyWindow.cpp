#include "MyWindow.h"
#include <stdexcept>
#include "gtc/matrix_transform.hpp"

MyWindow::MyWindow(int width, int height, const char* title)
	: windowSize(width, height), clearColor(1.0f, 1.0f, 1.0f, 1.0f), physicWorld(new PhysicWorld())
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
	renderer = new Renderer("res/shaders/Vertex.shader", "res/shaders/Fragment.shader");
	renderer->GetShader()->SetUniformMat4f("u_proj", glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f));
	renderer->GetShader()->SetUniformMat4f("u_view", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
	renderer->GetShader()->SetUniformBlock("u_TransForm", 0);
}

MyWindow::~MyWindow()
{
	glfwTerminate();
}

void MyWindow::AddImage(const std::string imagePath)
{
	ImageProperty imgProp;
	unsigned char* localBuffer = stbi_load(imagePath.c_str(), &imgProp.width, &imgProp.height, &imgProp.BPP, 4);
	if (!localBuffer) {
		images.pop_back();
		throw std::runtime_error("Failed to load texture: " + imagePath);
	}
	imgProp.slot = renderer->AddTexture(localBuffer, imgProp.width, imgProp.height);
	images.push_back(std::move(imgProp));
	stbi_image_free(localBuffer);
}

void MyWindow::SetClearColor(float r, float g, float b, float a)
{
	clearColor = glm::vec4(r, g, b, a);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

bool MyWindow::Loop()
{
	bool run = !glfwWindowShouldClose(window);
	if (run) {
		glClear(GL_COLOR_BUFFER_BIT);
		Update();
	}
	return run;
}

void MyWindow::Update()
{
	for (auto& model : models)
	{
		model->Update();
		model->Draw();
	}
}
