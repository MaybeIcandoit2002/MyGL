#include "MyWindow.h"
#include "RandomDevice.h"
#include "Macros.h"

#include <fstream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_internal.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include "vendor/json.hpp"

#include "tests/TestMenu.h"
#include "tests/AddNewObject.h"
#include "tests/EditProperties.h"
#include "tests/SearchRoot.h"
#include "tests/Setting.h"

inline static const ComponentTemplate* FindTemplateByName(const std::string& name)
{
    for (const auto& ct : Sources::Instance()->GetComponentTemplate())
    {
        if (ct.name == name) return &ct;
    }
    return nullptr;
}

inline static Component* CreateComponentByTemplate(MyWindow* window, const ComponentTemplate& ct)
{
    Mesh* mesh = Sources::Instance()->GetMesh(ct.meshName);
    if (!mesh || !window) return nullptr;

    Component* child = new Component(mesh);
    window->GetRootComponent()->AddChild(child);
    child->name = ct.name;
    child->shapeType = ct.shapeType;
    child->SetTextureSlot(ct.textureSlot);
    child->SetPosition(0, 0);
    child->SetScale(ct.scale1, ct.scale2);

    switch (ct.shapeType)
    {
    case ShapeType::Circle:
        child->InitPhysicProperty(ct.physicSize1, ct.physicMass, ct.physicRestitution, ct.physicFriction);
        break;
    case ShapeType::Box:
        child->InitPhysicProperty(ct.physicSize1, ct.physicSize2[0], ct.physicMass, ct.physicRestitution, ct.physicFriction);
        break;
    case ShapeType::Polygon:
        child->InitPhysicProperty(static_cast<int>(ct.physicSize1), ct.physicSize2, ct.physicMass, ct.physicRestitution, ct.physicFriction);
        break;
    }
    child->SetSensor(true);
    return child;
}

inline static void SaveScene(MyWindow* window, const std::string& path)
{
    if (!window) return;
    nlohmann::json scene;
    scene["objects"] = nlohmann::json::array();

    Component* root = window->GetRootComponent();
    for (Component* child : root->children)
    {
        if (!child) continue;
        glm::vec2 pos, scale, relPos, relScale;
        glm::vec4 color;
        float rot = 0.0f;
        child->GetPosition(pos);
        child->GetScale(scale);
        child->GetRotation(rot);
        child->GetBackgroundColor(color);
        child->GetPhysicRelativePosition(relPos);
        child->GetPhysicRelativeScale(relScale);

        nlohmann::json obj;
        obj["templateName"] = child->name;
        obj["position"] = { pos.x, pos.y };
        obj["scale"] = { scale.x, scale.y };
        obj["rotation"] = rot;
        obj["color"] = { color.x, color.y, color.z, color.w };
        obj["hasPhysicBody"] = child->hasPhysicBody;
        obj["shapeType"] = static_cast<int>(child->shapeType);
        obj["physicMass"] = child->physicMass;
        obj["physicSize1"] = child->physicSize1;
        obj["physicSize2"] = child->physicSize2 ? child->physicSize2[0] : 0.0f;
        obj["friction"] = child->GetFriction();
        obj["restitution"] = child->GetRestitution();
        obj["relativePos"] = { relPos.x, relPos.y };
        obj["relativeScale"] = { relScale.x, relScale.y };
        scene["objects"].push_back(obj);
    }

    std::ofstream out(path);
    if (out) out << scene.dump(2);
}

inline static void LoadScene(MyWindow* window, const std::string& path)
{
    if (!window) return;
    std::ifstream in(path);
    if (!in) return;

    nlohmann::json scene;
    in >> scene;

    Component* root = window->GetRootComponent();
    while (!root->children.empty())
    {
        Component* child = root->children.back();
        root->RemoveChild(child);
        delete child;
    }

    for (auto& obj : scene["objects"])
    {
        std::string templateName = obj.value("templateName", std::string());
        const ComponentTemplate* ct = FindTemplateByName(templateName);
        if (!ct) continue;

        Component* child = CreateComponentByTemplate(window, *ct);
        if (!child) continue;

        glm::vec2 pos(obj["position"][0].get<float>(), obj["position"][1].get<float>());
        glm::vec2 scale(obj["scale"][0].get<float>(), obj["scale"][1].get<float>());
        glm::vec4 color(obj["color"][0].get<float>(), obj["color"][1].get<float>(), obj["color"][2].get<float>(), obj["color"][3].get<float>());
        float rot = obj.value("rotation", 0.0f);
        child->SetPosition(pos);
        child->SetScale(scale);
        child->SetRotation(rot);
        child->SetBackgroundColor(color);

        child->shapeType = static_cast<ShapeType>(obj.value("shapeType", static_cast<int>(ct->shapeType)));
        child->physicMass = obj.value("physicMass", ct->physicMass);
        child->SetPhysicSize(obj.value("physicSize1", ct->physicSize1), obj.value("physicSize2", ct->physicSize2[0]));
        child->SetFriction(obj.value("friction", ct->physicFriction));
        child->SetRestitution(obj.value("restitution", ct->physicRestitution));
        child->SetPhysicRelativePosition(obj["relativePos"][0].get<float>(), obj["relativePos"][1].get<float>());
        child->SetPhysicRelativeScale(obj["relativeScale"][0].get<float>(), obj["relativeScale"][1].get<float>());
        child->SetSensor(!obj.value("hasPhysicBody", false));
    }

    window->selectedComponent = nullptr;
    window->changeSelectedComponent = true;
}

inline static void TestInitGui(MyWindow& window, test::TestMenu*& testMenu, test::Test*& currentTest) {
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.GetWindow(), false);
    ImGui::StyleColorsDark();

    currentTest = nullptr;
    testMenu = new test::TestMenu(&window);
    currentTest = testMenu;

    testMenu->RegisterTest<test::AddNewObject>("Add New Object");
    testMenu->RegisterTest<test::SearchRoot>("RootComponent");
    testMenu->RegisterTest<test::Setting>("Setting");
}

inline static void TestGui(const void* testObject, test::TestMenu* testMenu, test::Test*& currentTest) {
    ImGui_ImplGlfwGL3_NewFrame();
	MyWindow* window = testMenu->window;

	static bool timerEnabled = true;
	static double physicsRunTimer = 0.0;
	static float countdownSetSeconds = 10.0f;
	static float countdownRemainingSeconds = 0.0f;
	static bool countdownActive = false;

    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 display = io.DisplaySize;
        for (int i = 0; i < ImGui::GetCurrentContext()->Windows.Size; ++i)
        {
            ImGuiWindow* w = ImGui::GetCurrentContext()->Windows[i];
            if (!w)
                continue;

            const float maxX = display.x - w->SizeFull.x - style.DisplayWindowPadding.x;
            const float maxY = display.y - w->SizeFull.y - style.DisplayWindowPadding.y;
            ImVec2 clamped(
                w->Pos.x < style.DisplayWindowPadding.x ? style.DisplayWindowPadding.x : (w->Pos.x > maxX ? maxX : w->Pos.x),
                w->Pos.y < style.DisplayWindowPadding.y ? style.DisplayWindowPadding.y : (w->Pos.y > maxY ? maxY : w->Pos.y)
            );
            if (clamped.x != w->Pos.x || clamped.y != w->Pos.y)
                ImGui::SetWindowPos(w->Name, clamped, ImGuiCond_Always);
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 12.0f, 12.0f), 0, ImVec2(1.0f, 0.0f));
    ImGui::Begin("##ViewScaleOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoInputs);
    ImGui::Text("Scale: %.2fx", window->GetViewScale());
    ImGui::End();

	if (timerEnabled && window->running)
	{
		physicsRunTimer += ImGui::GetIO().DeltaTime;
	}

	if (countdownActive && window->running)
	{
		countdownRemainingSeconds -= ImGui::GetIO().DeltaTime;
		if (countdownRemainingSeconds <= 0.0f)
		{
			countdownRemainingSeconds = 0.0f;
			countdownActive = false;
			window->running = false;
		}
	}

	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), 0, ImVec2(0.0f, 0.0f));
	ImGui::Begin("##PhysicsRunTimer", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav);
	ImGui::Text("Timer: %.2f s", physicsRunTimer);
	ImGui::Checkbox("Enable Timer", &timerEnabled);
	ImGui::SameLine();
	if (ImGui::Button("Reset Timer"))
	{
		physicsRunTimer = 0.0;
	}

	ImGui::Separator();
	ImGui::Text("Countdown");
	if (window->selectedComponent == nullptr)
	{
		if (ImGui::DragFloat("Set Countdown (s)", &countdownSetSeconds, 0.1f, 0.1f, 36000.0f, "%.1f"))
		{
			if (countdownSetSeconds < 0.1f) countdownSetSeconds = 0.1f;
		}
		ImGui::Text("Remaining: %.2f s", countdownActive ? countdownRemainingSeconds : countdownSetSeconds);
		if (!countdownActive)
		{
			if (ImGui::Button("Start Countdown"))
			{
				countdownRemainingSeconds = countdownSetSeconds;
				countdownActive = true;
			}
		}
		else
		{
			if (ImGui::Button("Stop Countdown"))
			{
				countdownActive = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Countdown"))
		{
			countdownActive = false;
			countdownRemainingSeconds = countdownSetSeconds;
		}
	}
	else
	{
		ImGui::TextDisabled("Countdown setup available only when no object is selected.");
		ImGui::Text("Remaining: %.2f s", countdownActive ? countdownRemainingSeconds : countdownSetSeconds);
	}
	ImGui::End();

    if (!window->running)
    {
        window->DrawColliderOutline();
    }
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
        if (!window->running && ImGui::Button("Pause One Step"))
        {
            window->GetPhysicWorld()->Step(1.0 / 60.0);
        }

        PhysicWorld* physicWorld = window->GetPhysicWorld();
        ImGui::Separator();
        ImGui::Text("Global Physic Params");
        cpVect gravity = physicWorld->GetGravity();
        float gravityValue[2] = { static_cast<float>(gravity.x), static_cast<float>(gravity.y) };
        if (ImGui::DragFloat2("Gravity", gravityValue, 0.1f, -100.0f, 100.0f))
        {
            physicWorld->SetGravity(cpv(gravityValue[0], gravityValue[1]));
        }

        float staticFriction = static_cast<float>(physicWorld->GetStaticFrictionCoeff());
        if (ImGui::DragFloat("Static Friction Coeff", &staticFriction, 0.01f, 0.0f, 5.0f))
        {
            physicWorld->SetStaticFrictionCoeff(staticFriction);
        }

        float kineticFriction = static_cast<float>(physicWorld->GetKineticFrictionCoeff());
        if (ImGui::DragFloat("Kinetic Friction Coeff", &kineticFriction, 0.01f, 0.0f, 5.0f))
        {
            physicWorld->SetKineticFrictionCoeff(kineticFriction);
        }

        float slipThreshold = static_cast<float>(physicWorld->GetSlipSpeedThreshold());
        if (ImGui::DragFloat("Slip Speed Threshold", &slipThreshold, 0.01f, 0.0f, 20.0f))
        {
            physicWorld->SetSlipSpeedThreshold(slipThreshold);
        }

        if (ImGui::Button("Reset Global Physic Params"))
        {
            physicWorld->SetGravity(cpv(0, -9.8));
            physicWorld->SetStaticFrictionCoeff(1.2f);
            physicWorld->SetKineticFrictionCoeff(0.8f);
            physicWorld->SetSlipSpeedThreshold(0.5f);
        }

        if (ImGui::Button("Save Scene"))
        {
            SaveScene(window, "res/scenes/runtime_scene.json");
        }
        ImGui::SameLine();
        if (!window->running && ImGui::Button("Load Scene"))
        {
            LoadScene(window, "res/scenes/runtime_scene.json");
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

        Sources::Instance()->LoadImages("res/textures/ball.png", renderer); // 预加载纹理: 0
        Sources::Instance()->LoadImages("res/textures/car.png", renderer); // 预加载纹理: 1
        Sources::Instance()->LoadImages("res/textures/circle.png", renderer); // 预加载纹理: 2
        Sources::Instance()->LoadImages("res/textures/face_faint.png", renderer); // 预加载纹理: 3
        Sources::Instance()->LoadImages("res/textures/face_sad.png", renderer); // 预加载纹理: 4
        Sources::Instance()->LoadImages("res/textures/planet.png", renderer); // 预加载纹理: 5
        Sources::Instance()->LoadComponentFromJson("res/Components/default.json");
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