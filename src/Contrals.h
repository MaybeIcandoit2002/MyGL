#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "MyWindow.h"
#include "Models.h"
class Contrals
{
private:
	MyWindow* window;
    Contrals* parentCtrl;
	Contrals* childCtrl;
	Contrals* nextCtrl;

	glm::vec2 position;
    glm::mat4 sceneMat;

	std::vector<Models*> models;
	
public:
	Contrals(MyWindow* window) : window(window), parentCtrl(nullptr), childCtrl(nullptr), nextCtrl(nullptr), position(0.0f, 0.0f)
	{
		SetSceneSize((float)window->GetWidth(), (float)window->GetHeight());
	}
    Contrals(Contrals* parentCtrl);
    ~Contrals();
	void SetPosition(float x, float y) { position = glm::vec2(x, y); }
	void SetSceneSize(float width, float height) { sceneMat = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f); }

	void AddModels(Models* model);

	float GetSceneWidth() { return 2.0f / sceneMat[0][0]; }
	float GetSceneHeight() { return -2.0f / sceneMat[1][1]; }
	glm::mat4 GetSceneMat() { return sceneMat; }
};