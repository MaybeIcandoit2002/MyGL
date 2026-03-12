#include "MyWindow.h"
#include <stdexcept>
#include "gtc/matrix_transform.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

MyWindow::MyWindow(int width, int height, const char* title)
	: windowSize(width, height), defaultWindowSize(width, height), lastWindowSize(width, height), clearColor(1.0f, 1.0f, 1.0f, 1.0f), lastTime(0.0),
	viewScale(1.0f), windowScale(1.0f), viewCenter(0.0f, 0.0f),
	jointLineColor(1.0f, 0.86f, 0.31f, 1.0f), jointLineThickness(2.0f),
	renderer(renderer), selectedComponent(nullptr), changeSelectedComponent(false), bindingAnchorComponent(nullptr), autoHideEditProperties(true),
	draggingSelectedComponent(false), draggingOffset(0.0f, 0.0f), running(false)
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
		if (ImGui::GetIO().WantCaptureMouse) return;
		MyWindow* self = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
		if (!self) return;
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			self->draggingSelectedComponent = false;
			return;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			const glm::vec2 world = self->ScreenToWorld(xpos, ypos);
			DEBUG_PRINT("Mouse Clicked at: (" << world.x << ", " << world.y << ")");

			Component* picked = nullptr;
			for (Component* child : self->GetRootComponent()->children)
			{
				if (child->enabled && child->checkPointInShape(world.x, world.y))
				{
					DEBUG_PRINT("Component " << child->name << " selected.");
					picked = child;
					break;
				}
			}

			if (picked && picked == self->GetSelectedComponent() && !self->running)
			{
				glm::vec2 pickedWorldPos;
				picked->GetWorldPosition(pickedWorldPos);
				self->draggingOffset = pickedWorldPos - world;
				self->draggingSelectedComponent = true;
			}
			else
			{
				self->draggingSelectedComponent = false;
				self->SelectComponent(picked);
			}
		}
	});

	glfwSetCursorPosCallback(window,
	[](GLFWwindow* window, double xpos, double ypos)
	{
		MyWindow* self = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
		if (!self) return;
		if (ImGui::GetIO().WantCaptureMouse) return;
		if (!self->draggingSelectedComponent || self->running) return;
		Component* selected = self->GetSelectedComponent();
		if (!selected)
		{
			self->draggingSelectedComponent = false;
			return;
		}
		const glm::vec2 world = self->ScreenToWorld(xpos, ypos);
		selected->SetWorldPosition(world + self->draggingOffset);
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
			{
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				self->SetViewScale(self->GetViewScale() + (float)yoffset * 0.1f, xpos, ypos);
			}
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
		glm::ortho(viewCenter.x - halfW, viewCenter.x + halfW, viewCenter.y - halfH, viewCenter.y + halfH, -1.0f, 1.0f)
	);
}

void MyWindow::SetViewScale(float scale)
{
	if (scale < 0.5f) scale = 0.5f;
	if (scale > 3.0f) scale = 3.0f;
	viewScale = scale;
	RefreshProjection();
}

void MyWindow::SetViewScale(float scale, double mouseX, double mouseY)
{
	const glm::vec2 before = ScreenToWorld(mouseX, mouseY);
	SetViewScale(scale);
	const glm::vec2 after = ScreenToWorld(mouseX, mouseY);
	viewCenter += (before - after);
	RefreshProjection();
}

void MyWindow::SetViewCenter(const glm::vec2& center)
{
	viewCenter = center;
	RefreshProjection();
}

void MyWindow::ResetView()
{
	viewScale = 1.0f;
	viewCenter = glm::vec2(0.0f, 0.0f);
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

glm::vec2 MyWindow::ScreenToWorld(double screenX, double screenY) const
{
	return glm::vec2(
		viewCenter.x + static_cast<float>((screenX - windowSize.x * 0.5f) / viewScale),
		viewCenter.y + static_cast<float>((windowSize.y * 0.5f - screenY) / viewScale)
	);
}

glm::vec2 MyWindow::WorldToScreen(const cpVect& world) const
{
	return glm::vec2(
		windowSize.x * 0.5f + (static_cast<float>(world.x) - viewCenter.x) * viewScale,
		windowSize.y * 0.5f - (static_cast<float>(world.y) - viewCenter.y) * viewScale
	);
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
	const glm::vec2 center = GetViewCenter();
	rootComponent->DrawColliderOutlineRecursive(drawList, GetWidth(), GetHeight(), GetViewScale(), center.x, center.y, IM_COL32(80, 220, 120, 255));
}

void MyWindow::DrawJointBindings()
{
	if (!physicWorld) return;
	ImDrawList* drawList = ImGui::GetOverlayDrawList();
	if (!drawList) return;

	const glm::vec4 c = jointLineColor;
	const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(c.r, c.g, c.b, c.a));
	const float thickness = (jointLineThickness < 0.5f) ? 0.5f : jointLineThickness;

	const std::vector<std::pair<cpVect, cpVect>> segments = physicWorld->GetJointWorldSegments();
	for (const auto& segment : segments)
	{
		const glm::vec2 a = WorldToScreen(segment.first);
		const glm::vec2 b = WorldToScreen(segment.second);
		drawList->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), color, thickness);
	}
}

void MyWindow::SelectComponent(Component* component)
{
	draggingSelectedComponent = false;
	if (selectedComponent == component) return;
	selectedComponent = component;
	changeSelectedComponent = true;
}

void MyWindow::ClearSelection()
{
	SelectComponent(nullptr);
}

void MyWindow::AcknowledgeSelectionChanged()
{
	changeSelectedComponent = false;
}

void MyWindow::ClearBindingAnchorIf(Component* component)
{
	if (bindingAnchorComponent == component)
		bindingAnchorComponent = nullptr;
}
