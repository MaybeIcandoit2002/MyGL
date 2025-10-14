#include "TestScene.h"
#include "../Models.h"
namespace test {
    TestScene::TestScene()
    {

	}
    void TestScene::OnUpdate(const void* p, float deltaTime)
    {
        Models* models = (Models*)p;
		ImGui::Text("models position x: %.2f, y: %.2f", models->GetPosition().matRT[2][0], models->GetPosition().matRT[2][1]);
    }
    void TestScene::OnRender()
    {
    }
    void TestScene::OnImGuiRender()
    {
		ImGui::Text("Test Scene");
    }
}