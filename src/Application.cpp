#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "MyWindow.h"
#include "Contrals.h"
#include "Models.h"
#include "LoadingModelsFormJSON.h"
#include "RandomDevice.h"

#include "renderComponents/Renderer.h"
#include "renderComponents/ShaderProgram.h"
#include "renderComponents/Texture.h"

#include "collisionSystem/PhysicWorld.h"

#include "vendor/stb_image/stb_image.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestMenu.h"
#include "tests/TestScene.h"
int main(void)
{
	RandomDevice randomDevice;
	int Width = 1920, Height = 1080;
    {
		MyWindow window(Width, Height, "Hello World");
		window.SetClearColor(0.9f, 0.9f, 0.9f, 1.0f);

		PhysicWorld physicWorld;
		physicWorld.SetGravity(cpv(0.0f, 8.0f));
        physicWorld.SetBoundaryLine(cpv(-10.0f, 0.0f), cpv((float)Width + 10.0f, 0.0f));
        physicWorld.SetBoundaryLine(cpv(0.0f, -10.0f), cpv(0.0f, (float)Height + 10.0f));
        physicWorld.SetBoundaryLine(cpv((float)Width, (float)Height + 10.0f), cpv((float)Width, -10.0f));
        physicWorld.SetBoundaryLine(cpv((float)Width + 10.0f, (float)Height), cpv(-10.0f, (float)Height));

		Contrals Ctrl;

		Ctrl.SetSceneSize((float)Width, (float)Height);

		Renderer renderer;

		ShaderProgram shader;
        shader.Add(GL_VERTEX_SHADER, "res/shaders/Vertex.shader");
        shader.Add(GL_FRAGMENT_SHADER, "res/shaders/Fragment.shader");
		shader.Link();
        Texture texture1("res/textures/star.png");
        Texture texture2("res/textures/pic1.png");
        texture1.Bind(0);
        texture2.Bind(1);
		shader.SetUniformMat4f("u_proj", Ctrl.GetSceneMat());
		shader.SetUniformMat4f("u_view", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
        int l[2] = { 0, 1 };
        shader.SetUniform1iv("u_Textures", 2, l);
        shader.SetUniformBlock("u_TransForm", 0);

        VertexBufferLayout layout;
		layout.Push<float>(2, GL_FALSE);
		layout.Push<float>(4, GL_FALSE);
		layout.Push<float>(2, GL_FALSE);
		layout.Push<float>(1, GL_FALSE);
		std::vector<Models*> modelsSource = std::vector<Models*>();
        {
            Models* m;
            LoadModelsFromJson("res/models/models.json", "stars", &layout, &shader, m);
			modelsSource.push_back(m);
        }
		std::vector<Models*> models = std::vector<Models*>();
		for (int i = 0; i < 9; i++)
        {
            models.push_back(new Models(*modelsSource[0], &layout));
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                models[i * 3 + j]->MoveTo((float)(i+1) * (200.f) + randomDevice.Float(-10, 10), (float)(j + 1) * 200.f + randomDevice.Float(-10, 10));
				physicWorld.AddCircle(*models[i * 3 + j]);
            }
        }

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window.GetWindow(), true);
        ImGui::StyleColorsDark();

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestScene>("Test Scene");

        /*float view_X = 0.0f;
        float view_Y = 0.0f;
        float view_moveV = 1.5f;
        float followSapce = 100.0f;
        bool followPoint = true;
        float followSpeed = 0.1f;
        float x = 0.0f, y = 0.0f;*/
        while (!window.ShouldClose())
        {
            /* Render here */
            ImGui_ImplGlfwGL3_NewFrame();
            if (currentTest) {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                currentTest->OnUpdate(&models, 0.0f);
                currentTest->OnRender();
                currentTest->OnImGuiRender();
                if (currentTest != testMenu && ImGui::Button("<-")) {
                    delete currentTest;
                    currentTest = testMenu;
                }
            }
			physicWorld.Step(0.05);
            for (auto& m : models)
            {
				m->UpdatePhysicFromModel();
                m->Draw(&renderer);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
            window.End();
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
        delete currentTest;
        if (currentTest != testMenu)
            delete testMenu;
        ImGui_ImplGlfwGL3_Shutdown();
        ImGui::DestroyContext();
    }
    return 0;
}