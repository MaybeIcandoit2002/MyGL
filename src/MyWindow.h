#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "renderComponents/renderer.h"
#include "collisionSystem/PhysicWorld.h"

#include "Models.h"
class MyWindow
{
private:
	GLFWwindow* window;
	PhysicWorld* physicWorld;
	Renderer* render;
	glm::vec2 windowSize;
	glm::vec4 clearColor;
	std::vector<std::pair<uint32_t, std::string>> pictures;

	std::vector<Models*> models;
public:
	MyWindow(int width, int height, const char* title);
	~MyWindow();
	void AddModel(Models* model) { models.push_back(model); }
	void AddPicture(const std::string imagePath, uint32_t slot) { pictures.push_back({ slot, imagePath }); }
	void ZippedPictures() { render->InitializeTextures(pictures); }

	void SetClearColor(float r, float g, float b, float a);

	GLFWwindow* GetWindow() { return window; }
	PhysicWorld* GetPhysicWorld() { return physicWorld; }
	Renderer* GetRenderer() { return render; }
	std::vector<Models*>& GetModels() { return models; }
	float GetWidth() { return windowSize[0]; }
	float GetHeight() { return windowSize[1]; }

	bool Loop();
	void LoopEnd() { glfwSwapBuffers(window); glfwPollEvents(); }
	void Update();
};

