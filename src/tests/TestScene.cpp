#include "TestScene.h"
namespace test {
    TestScene::TestScene()
    {

	}
    void TestScene::OnUpdate(const void* p, float deltaTime)
    {
    }
    void TestScene::OnRender()
    {
    }
    void TestScene::OnImGuiRender()
    {
		ImGui::Text("Test Scene");
    }
}