#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <glm.hpp>

#include "MyWindow.h"
#include "RandomDevice.h"
#include "Macros.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestMenu.h"
#include "tests/TestScene.h"

inline static void TestInitGui(MyWindow& window, test::TestMenu*& testMenu, test::Test*& currentTest) {
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.GetWindow(), true);
    ImGui::StyleColorsDark();

    currentTest = nullptr;
    testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    testMenu->RegisterTest<test::TestScene>("Test Scene");
}

inline static void TestGui(const void* testObject, test::Test* testMenu, test::Test* currentTest) {
    ImGui_ImplGlfwGL3_NewFrame();
    if (currentTest) {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        currentTest->OnUpdate(testObject, 0.0f);
        currentTest->OnRender();
        currentTest->OnImGuiRender();
        if (currentTest != testMenu && ImGui::Button("<-")) {
            delete currentTest;
            currentTest = testMenu;
        }
    }
    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}
int main(void)
{
	RandomDevice randomDevice;
	int Width = 1080, Height = 720;
    {
		MyWindow window(Width, Height, "Hello World");
		window.SetClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        //window.AddImage("res/textures/pic1.png");

		PhysicWorld* physicWorld = window.GetPhysicWorld();
		physicWorld->SetGravity(cpv(0.0f, 9.8f)); 
        physicWorld->SetBoundaryLine(cpv(-10.0f, 0.0f), cpv((float)Width + 10.0f, 0.0f));
        physicWorld->SetBoundaryLine(cpv(0.0f, -10.0f), cpv(0.0f, (float)Height + 10.0f));
        physicWorld->SetBoundaryLine(cpv((float)Width, (float)Height + 10.0f), cpv((float)Width, -10.0f));
        physicWorld->SetBoundaryLine(cpv((float)Width + 10.0f, (float)Height), cpv(-10.0f, (float)Height));

        utils::LoadImages("res/textures/star.png", window.GetRenderer());
        utils::LoadMeshsFromJson("res/meshs/default.json", window.GetRenderer());

		Component* root = window.GetRootComponent();
        Component* background = new Component(false, Sources::GetInstance()->GetMesh("rootMesh"));
        Component* rightBar = new Component(false, Sources::GetInstance()->GetMesh("rootMesh"));
        root->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
		root->AddChild(background);
        root->AddChild(rightBar);
        background->SetScale(Width, Height);
		background->SetBackgroundColor(0.9f, 0.9f, 0.9f, 1.0f);
        rightBar->SetPosition(Width - 200.0f, 0);
		rightBar->SetScale(200.0f, Height);
        rightBar->SetBackgroundColor(0.5f, 0.5f, 0.5f, 1.0f);


        glm::vec2 pos;
        window.GetRenderer()->SendToGPU();
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = nullptr;
        TestInitGui(window, testMenu, currentTest);

        double deltaTime;
        while (window.Loop(deltaTime))
        {

            /*rightBar->GetWorldPosition(pos);
            pos.x += 50.0f * deltaTime;
			if (pos.x > Width/2-100.0f) pos.x -= (float)Width/2;
            rightBar->SetWorldPosition(pos);*/
            DEBUG_RUN(TestGui(nullptr, testMenu, currentTest));
            window.LoopEnd();
            /*if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
                followPoint = false;
                Vs[26].position[0] = Vs[26].position[0] + view_moveV;
                Vs[27].position[0] = Vs[27].position[0] + view_moveV;
                Vs[28].position[0] = Vs[28].position[0] + view_moveV;
                Vs[29].position[0] = Vs[29].position[0] + view_moveV;
                if (view_X < followSapce)
                    view_X += view_moveV;
                else
                    x = x + view_moveV;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT)) {
                followPoint = false;
                Vs[26].position[0] = Vs[26].position[0] - view_moveV;
                Vs[27].position[0] = Vs[27].position[0] - view_moveV;
                Vs[28].position[0] = Vs[28].position[0] - view_moveV;
                Vs[29].position[0] = Vs[29].position[0] - view_moveV;
                if (view_X > -followSapce)
                    view_X -= view_moveV;
                else
                    x = x - view_moveV;
            }
            if (glfwGetKey(window, GLFW_KEY_UP)) {
                followPoint = false;
                Vs[26].position[1] = Vs[26].position[1] - view_moveV;
                Vs[27].position[1] = Vs[27].position[1] - view_moveV;
                Vs[28].position[1] = Vs[28].position[1] - view_moveV;
                Vs[29].position[1] = Vs[29].position[1] - view_moveV;
                if (view_Y > -followSapce)
                    view_Y -= view_moveV;
                else
                    y = y - view_moveV;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN)) {
                followPoint = false;
                Vs[26].position[1] = Vs[26].position[1] + view_moveV;
                Vs[27].position[1] = Vs[27].position[1] + view_moveV;
                Vs[28].position[1] = Vs[28].position[1] + view_moveV;
                Vs[29].position[1] = Vs[29].position[1] + view_moveV;
                if (view_Y < followSapce)
                    view_Y += view_moveV;
                else
                    y = y + view_moveV;
            }
            if (followPoint)
            {
                if (abs(view_X) < 1.0f)
                {
                    x = x + view_X;
                    view_X = 0.0f;
                }
                else
                {
                    x = x + view_X * followSpeed;
                    view_X = view_X * (1 - followSpeed);
                }
                if (abs(view_Y) < 1.0f)
                {
                    y = y + view_Y;
                    view_Y = 0.0f;
                }
                else
                {
                    y = y + view_Y * followSpeed;
                    view_Y = view_Y * (1 - followSpeed);
                }
            }*/
        }
        if (currentTest != testMenu)
            delete testMenu;
        delete currentTest;
        ImGui_ImplGlfwGL3_Shutdown();
        ImGui::DestroyContext();
		window.LoopEnd();
    }
    return 0;
}