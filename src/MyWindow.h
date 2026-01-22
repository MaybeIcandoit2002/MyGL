#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "glm.hpp"
#include "renderComponents/renderer.h"
#include "collisionSystem/PhysicWorld.h"
#include "./vendor/stb_image/stb_image.h"

#include "Components/Component.h"
class MyWindow
{
private:
	GLFWwindow* window;
	PhysicWorld* physicWorld;
	Renderer* renderer;
	glm::vec2 windowSize;
	glm::vec4 clearColor;
	Component* rootComponent;
public:
	MyWindow(int width, int height, const char* title);
	~MyWindow();

	void SetClearColor(float r, float g, float b, float a);

	inline GLFWwindow* GetWindow() { return window; }
	inline PhysicWorld* GetPhysicWorld() { return physicWorld; }
	inline Renderer* GetRenderer() { return renderer; }
	inline Component* GetRootComponent() { return rootComponent; }
	inline float GetWidth() { return windowSize[0]; }
	inline float GetHeight() { return windowSize[1]; }

	bool Loop();
	void LoopEnd() { glfwSwapBuffers(window); glfwPollEvents(); }
	void Update();
};