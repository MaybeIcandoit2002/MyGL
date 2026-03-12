#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "renderComponents/renderer.h"
#include "collisionSystem/PhysicWorld.h"

#include "Components/Component.h"
class MyWindow
{
private:
	GLFWwindow* window;
	PhysicWorld* physicWorld;
	Renderer* renderer;
	glm::vec2 windowSize;
	glm::vec2 defaultWindowSize;
	glm::vec2 lastWindowSize;
	glm::vec4 clearColor;
	Component* rootComponent;
	double lastTime;
	float viewScale;
	float windowScale;
	void RefreshProjection();
public:
	bool running;
	bool changeSelectedComponent;
	Component* selectedComponent;

	MyWindow(int width, int height, const char* title);
	~MyWindow();

	void SetClearColor(float r, float g, float b, float a);

	inline GLFWwindow* GetWindow() { return window; }
	inline PhysicWorld* GetPhysicWorld() { return physicWorld; }
	inline Renderer* GetRenderer() { return renderer; }
	inline Component* GetRootComponent() { return rootComponent; }
	inline float GetWidth() { return windowSize[0]; }
	inline float GetHeight() { return windowSize[1]; }
	inline float GetViewScale() const { return viewScale; }
	inline float GetWindowScale() const { return windowScale; }
	void SetViewScale(float scale);
	void SetWindowScale(float scale);

	bool Loop(double& deltaTime);
	void LoopEnd() { glfwSwapBuffers(window); glfwPollEvents(); }
	void BeforeUpdate();
	void Update();
	void DrawColliderOutline();
};