#include "MyWindow.h"
#include <stdexcept>
#include "gtc/matrix_transform.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

MyWindow::MyWindow(int width, int height, const char* title)
	: windowSize(width, height), defaultWindowSize(width, height), lastWindowSize(width, height), clearColor(1.0f, 1.0f, 1.0f, 1.0f), lastTime(0.0),
	viewScale(1.0f), windowScale(1.0f),
	renderer(renderer), running(false),
	selectedComponent(nullptr), changeSelectedComponent(false)
{
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
	renderer->GetShader()->SetUniformMat4f("u_proj", glm::ortho(-(float)width * 0.5f, (float)width * 0.5f, -(float)height * 0.5f, (float)height * 0.5f, -1.0f, 1.0f));
	renderer->GetShader()->SetUniformMat4f("u_view", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
	renderer->GetShader()->SetUniformBlock("u_TransForm", 0);

	Sources::Instance()->LoadMeshsFromJson("res/meshs/default.json", renderer);
	Sources::Instance()->LoadComponentFromJson("res/Components/default.json");
	rootComponent = new Component(
		utils::CopyMesh("root", Sources::Instance()->GetMesh("box"),
		{
			glm::vec2(width * 0.9f, height * 0.9f),
			glm::vec4(1.0, 1.0, 1.0f, 1.0f),
			glm::vec2(1, 1),
			-1
		},
		renderer));
	rootComponent->SetPosition(0.0f, 0.0f);

	PhysicWorld::Initialize(physicWorld, (float)width, (float)height);
	PhysicWorld::Instance()->SetStaticFrictionCoeff(1.2f);
	PhysicWorld::Instance()->SetKineticFrictionCoeff(0.8f);
	PhysicWorld::Instance()->SetSlipSpeedThreshold(0.5f);
	physicWorld->SetGravity(cpv(0, -9.8));

	// 注册 GLFW 鼠标点击回调
	glfwSetWindowUserPointer(window, this);
	glfwSetMouseButtonCallback(window,
	[](GLFWwindow* window, int button, int action, int mods)
	{
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
		if (ImGui::GetIO().WantCaptureMouse) return; // 如果 ImGui 捕获了鼠标事件, 则不处理点击事件
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			MyWindow* self = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			xpos = (xpos - self->GetWidth() * 0.5) / self->GetViewScale();
			ypos = (self->GetHeight() * 0.5 - ypos) / self->GetViewScale();
			DEBUG_PRINT("Mouse Clicked at: (" << xpos << ", " << ypos << ")");
			// 处理鼠标点击事件
			self->changeSelectedComponent = true;
			self->selectedComponent = nullptr;
			for (Component* child : self->rootComponent->children)
			{
				if (child->enabled && child->checkPointInShape(float(xpos), float(ypos)))
				{
					DEBUG_PRINT("Component " << child->name << " selected.");
					self->selectedComponent = const_cast<Component*>(child);
					break;
				}
			}
		}
	});

	glfwSetScrollCallback(window,
	[](GLFWwindow* window, double xoffset, double yoffset)
	{
		if (ImGui::GetCurrentContext())
			ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
		if (!ImGui::GetCurrentContext() || !ImGui::GetIO().WantCaptureMouse)
		{
			MyWindow* self = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
			if (self)
				self->SetViewScale(self->GetViewScale() + (float)yoffset * 0.1f);
		}
	});

	glfwSetWindowSizeCallback(window,
	[](GLFWwindow* window, int newWidth, int newHeight)
	{
		MyWindow* self = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
		if (!self) return;
		self->windowSize = glm::vec2((float)newWidth, (float)newHeight);
		glViewport(0, 0, newWidth, newHeight);
		self->RefreshProjection();
	});

	glfwSetKeyCallback(window,
	[](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (ImGui::GetCurrentContext())
			ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	});

	glfwSetCharCallback(window,
	[](GLFWwindow* window, unsigned int c)
	{
		if (ImGui::GetCurrentContext())
			ImGui_ImplGlfw_CharCallback(window, c);
	});

	/*glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* window, int newWidth, int newHeight)
		{
			float aspect = 1920.0f / 1080.0f;
			if (newWidth / newHeight > aspect)
			{
				glViewport(0, 0, newWidth, newWidth / aspect);
			}
			else
			{
				glViewport(0, 0, newHeight * aspect, newHeight);
			}
		});*/
}

MyWindow::~MyWindow()
{
	glfwTerminate();
}

void MyWindow::SetClearColor(float r, float g, float b, float a)
{
	clearColor = glm::vec4(r, g, b, a);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void MyWindow::RefreshProjection()
{
	const float halfW = windowSize.x * 0.5f / viewScale;
	const float halfH = windowSize.y * 0.5f / viewScale;
	renderer->GetShader()->SetUniformMat4f(
		"u_proj",
		glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f)
	);
}

void MyWindow::SetViewScale(float scale)
{
	if (scale < 0.5f) scale = 0.5f;
	if (scale > 3.0f) scale = 3.0f;
	viewScale = scale;
	RefreshProjection();
}

void MyWindow::SetWindowScale(float scale)
{
	if (scale < 1.0f) scale = 1.0f;
	if (scale > 2.5f) scale = 2.5f;
	windowScale = scale;
	const int w = (int)(defaultWindowSize.x * windowScale);
	const int h = (int)(defaultWindowSize.y * windowScale);
	glfwSetWindowSize(window, w, h);
}

bool MyWindow::Loop(double& deltaTime)
{
	deltaTime = glfwGetTime() - lastTime;
	lastTime = glfwGetTime();
	
	if (running) physicWorld->Step(deltaTime);
	bool run = !glfwWindowShouldClose(window);
	if (run) {
		glClear(GL_COLOR_BUFFER_BIT);
		BeforeUpdate();
		Update();
		Sources::Instance()->DrawOver();
	}
	return run;
}

void MyWindow::BeforeUpdate()
{
	rootComponent->BeforeUpdate(renderer, running);
}

void MyWindow::Update()
{
	rootComponent->Update(renderer);
}

void MyWindow::DrawColliderOutline()
{
	if (!rootComponent) return;
	ImDrawList* drawList = ImGui::GetOverlayDrawList();
	rootComponent->DrawColliderOutlineRecursive(drawList, GetWidth(), GetHeight(), GetViewScale(), IM_COL32(80, 220, 120, 255));
}
