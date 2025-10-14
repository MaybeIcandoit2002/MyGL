#include "Contrals.h"
#include "gtc/matrix_transform.hpp"

Contrals::Contrals()
	: parentCtrl(nullptr), childFirstCtrl(nullptr), childLastCtrl(nullptr), nextCtrl(nullptr), sceneMat(glm::mat4(1.0f))
{
}

Contrals::Contrals(Contrals* parentCtrl)
	: parentCtrl(parentCtrl), childFirstCtrl(nullptr), childLastCtrl(nullptr), sceneMat(glm::mat4(1.0f))
{
	if (parentCtrl->childFirstCtrl != nullptr) {
		nextCtrl = parentCtrl->childFirstCtrl;
		parentCtrl->childLastCtrl->nextCtrl = this;
	}
	else
	{
		nextCtrl = this;
	}
	parentCtrl->childFirstCtrl = this;
}

Contrals::~Contrals()
{
	Contrals *Ctrl, *next;
	for (Ctrl = childFirstCtrl; Ctrl != childFirstCtrl; Ctrl = next) {
		next = Ctrl->nextCtrl;
		delete Ctrl;
	}
}

void Contrals::SetSceneSize(float width, float height)
{
	sceneMat = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}
