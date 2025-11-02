#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "renderComponents/renderer.h"
#include "collisionSystem/PhysicWorld.h"
#include "./vendor/stb_image/stb_image.h"

#include "Models.h"
typedef struct { uint16_t slot; int width; int height; int BPP; } ImageProperty;
class MyWindow
{
private:
	GLFWwindow* window;
	PhysicWorld* physicWorld;
	Renderer* renderer;
	glm::vec2 windowSize;
	glm::vec4 clearColor;
	std::vector<ImageProperty> images;

	std::vector<Models*> models;
public:
	MyWindow(int width, int height, const char* title);
	~MyWindow();
	void AddModel(Models* model) { models.push_back(model); }
	void AddImage(const std::string imagePath);

	void SetClearColor(float r, float g, float b, float a);

	GLFWwindow* GetWindow() { return window; }
	PhysicWorld* GetPhysicWorld() { return physicWorld; }
	Renderer* GetRenderer() { return renderer; }
	std::vector<Models*>& GetModels() { return models; }
	inline float GetWidth() { return windowSize[0]; }
	inline float GetHeight() { return windowSize[1]; }

	bool Loop();
	void LoopEnd() { glfwSwapBuffers(window); glfwPollEvents(); }
	void Update();
};

