#include "MyWindow.h"
#include "RandomDevice.h"
#include "Macros.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestMenu.h"
#include "tests/AddNewObject.h"
#include "tests/EditProperties.h"

inline static void TestInitGui(MyWindow& window, test::TestMenu*& testMenu, test::Test*& currentTest) {
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.GetWindow(), false);
    ImGui::StyleColorsDark();

    currentTest = nullptr;
    testMenu = new test::TestMenu(&window);
    currentTest = testMenu;

    testMenu->RegisterTest<test::AddNewObject>("Add New Object");
}

inline static void TestGui(const void* testObject, test::TestMenu* testMenu, test::Test*& currentTest) {
    ImGui_ImplGlfwGL3_NewFrame();
	MyWindow* window = testMenu->window;
    if (currentTest) {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (window->running)
        {
            if (ImGui::Button("Stop PhysicSystem Progress"))
            {
                window->running = false;
            }
        }
        else
        {
            if (ImGui::Button("Run PhysicSystem Progress"))
            {
                window->running = true;
            }
        }
        if (window->changeSelectedComponent)
        {
			window->changeSelectedComponent = false;
            if (currentTest != testMenu) delete currentTest;
            if (window->selectedComponent)
            {
                currentTest = new test::EditProperties();
            }
            else
            {
                currentTest = testMenu;
            }
		}
        currentTest->OnUpdate(testObject, 0.0f);
        currentTest->OnRender();
        test::Test* newTest = currentTest->OnImGuiRender(window);
        if (newTest != nullptr)
        {
            if (currentTest != testMenu) delete currentTest;
            currentTest = newTest;
			newTest->OnImGuiRender(window);
        }
        if (currentTest != testMenu && ImGui::Button("<-")) {
            delete currentTest;
            currentTest = testMenu;
			window->selectedComponent = nullptr;
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
        MyWindow* window = nullptr;
        Renderer* renderer = nullptr;
        PhysicWorld* physicWorld = nullptr;

        window = new MyWindow(Width, Height, "Hello World");
        renderer = window->GetRenderer();
        physicWorld = window->GetPhysicWorld();

		Sources::Instance()->LoadImages("res/textures/star.png", renderer); // Ô¤¼ÓÔØÎÆÀí: 0
        window->SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        renderer->SendToGPU();
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = nullptr;
        TestInitGui(*window, testMenu, currentTest);

        double deltaTime;
        while (window->Loop(deltaTime))
        {
            /*rightBar->GetWorldPosition(pos);
            pos.x += 50.0f * deltaTime;
			if (pos.x > Width/2-100.0f) pos.x -= (float)Width/2; 
            rightBar->SetWorldPosition(pos);*/
            DEBUG_RUN(TestGui(nullptr, testMenu, currentTest));
            window->LoopEnd();
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
		window->LoopEnd();
    }
    return 0;
}