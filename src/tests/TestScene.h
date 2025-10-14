#pragma once
#include "Test.h"
#include <glm.hpp>
namespace test {
    class TestScene : public Test
    {
	public:
        TestScene();
        void OnUpdate(const void* p, float deltaTime) override;
        void OnRender() override;
		void OnImGuiRender() override;
    };
}

