#include "Contrals.h"

Contrals::Contrals(Contrals* parentCtrl)
	: window(parentCtrl->window), parentCtrl(parentCtrl), childCtrl(nullptr), position(parentCtrl->position), sceneMat(parentCtrl->sceneMat)
{
	nextCtrl = parentCtrl->childCtrl;
	parentCtrl->childCtrl = this;
}

Contrals::~Contrals()
{
	Contrals *Ctrl, *next;
	for (Ctrl = childCtrl; Ctrl != nullptr; Ctrl = next) {
		next = Ctrl->nextCtrl;
		delete Ctrl;
	}
}

void Contrals::AddModels(Models* model)
{
	models.push_back(model);
}
