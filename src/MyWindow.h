#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
class MyWindow
{
private:
	GLFWwindow* window;
	int width;
	int height;
	glm::vec4 clearColor;
public:
	MyWindow(int width, int height, const char* title);
	~MyWindow();
	bool ShouldClose() const;
	void End() const;
	void SetClearColor(float r, float g, float b, float a);
	void Update();
	GLFWwindow* GetWindow() { return window; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }
};

