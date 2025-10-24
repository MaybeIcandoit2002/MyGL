#include "TestScene.h"
#include "../Models.h"
namespace test {
    TestScene::TestScene()
    {

	}
    void TestScene::OnUpdate(const void* p, float deltaTime)
    {
        std::vector<Models*>* models = (std::vector<Models*>*)p;
        for (int i = 0; i < 9; i++)
        {
			ImGui::Text("Model %d Position: (%.2f, %.2f)", i, (*models)[i]->GetPoses()[0], (*models)[i]->GetPoses()[1]);
		}
    }
    void TestScene::OnRender()
    {
    }
    void TestScene::OnImGuiRender()
    {
		ImGui::Text("Test Scene");
    }
}