#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "renderComponents/renderer.h"

#include "Contrals.h"
#include "Models.h"
class MyWindow
{
private:
	GLFWwindow* window;
	Renderer* render;
	glm::vec2 windowSize;
	glm::vec4 clearColor;
	std::vector<std::pair<uint32_t, std::string>> pictures;
public:
	MyWindow(int width, int height, const char* title);
	~MyWindow();
	void AddPicture(const std::string imagePath, uint32_t slot);
	void AddModels(Models model);

	bool ShouldClose() const;
	void End() const;
	void SetClearColor(float r, float g, float b, float a);
	void Update();
	GLFWwindow* GetWindow() { return window; }
	int GetWidth() { return windowSize[0]; }
	int GetHeight() { return windowSize[1]; }
};

