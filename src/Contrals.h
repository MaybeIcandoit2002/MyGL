#pragma once
#include "glm.hpp"
class Contrals
{
private:
    Contrals* parentCtrl;
	Contrals* childFirstCtrl;
	Contrals* childLastCtrl;
	Contrals* nextCtrl;
    glm::mat4 sceneMat;
public:
    Contrals();
    Contrals(Contrals* parentCtrl);
    ~Contrals();
	void SetSceneSize(float width, float height);
	float GetSceneWidth() { return 2.0f / sceneMat[0][0]; }
	float GetSceneHeight() { return -2.0f / sceneMat[1][1]; }
	glm::mat4 GetSceneMat() { return sceneMat; }
};