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
	glm::vec2 viewCenter;
	glm::vec4 jointLineColor;
	float jointLineThickness;
	Component* selectedComponent;
	bool changeSelectedComponent;
	Component* bindingAnchorComponent;
	bool autoHideEditProperties;
	bool enableConfirmPopup;
	bool draggingSelectedComponent;
	glm::vec2 draggingOffset;
	bool draggingView;
	glm::vec2 lastCursorPos;
	void RefreshProjection();
public:
	bool running;

	MyWindow(int width, int height, const char* title);
	~MyWindow();

	void SetClearColor(float r, float g, float b, float a);
	inline glm::vec4 GetClearColor() const { return clearColor; }
	inline bool GetAutoHideEditProperties() const { return autoHideEditProperties; }
	inline void SetAutoHideEditProperties(bool enable) { autoHideEditProperties = enable; }
	inline bool GetEnableConfirmPopup() const { return enableConfirmPopup; }
	inline void SetEnableConfirmPopup(bool enable) { enableConfirmPopup = enable; }

	inline GLFWwindow* GetWindow() { return window; }
	inline PhysicWorld* GetPhysicWorld() { return physicWorld; }
	inline Renderer* GetRenderer() { return renderer; }
	inline Component* GetRootComponent() { return rootComponent; }
	inline float GetWidth() { return windowSize[0]; }
	inline float GetHeight() { return windowSize[1]; }
	inline float GetViewScale() const { return viewScale; }
	inline float GetWindowScale() const { return windowScale; }
	inline glm::vec2 GetViewCenter() const { return viewCenter; }
	void SetViewScale(float scale);
	void SetViewScale(float scale, double mouseX, double mouseY);
	void SetViewCenter(const glm::vec2& center);
	void ResetView();
	void SetWindowScale(float scale);
	glm::vec2 ScreenToWorld(double screenX, double screenY) const;
	glm::vec2 WorldToScreen(const cpVect& world) const;

	inline void SetJointLineColor(const glm::vec4& color) { jointLineColor = color; }
	inline glm::vec4 GetJointLineColor() const { return jointLineColor; }
	inline void SetJointLineThickness(float thickness) { jointLineThickness = thickness; }
	inline float GetJointLineThickness() const { return jointLineThickness; }

	void SelectComponent(Component* component);
	inline Component* GetSelectedComponent() const { return selectedComponent; }
	inline bool HasSelectionChanged() const { return changeSelectedComponent; }
	void ClearSelection();
	void AcknowledgeSelectionChanged();

	inline void SetBindingAnchor(Component* component) { bindingAnchorComponent = component; }
	inline Component* GetBindingAnchor() const { return bindingAnchorComponent; }
	inline void ClearBindingAnchor() { bindingAnchorComponent = nullptr; }
	void ClearBindingAnchorIf(Component* component);

	bool Loop(double& deltaTime);
	void LoopEnd() { glfwSwapBuffers(window); glfwPollEvents(); }
	void BeforeUpdate();
	void Update();
	void DrawColliderOutline();
	void DrawJointBindings();
	void DrawDescriptionLabels();
};