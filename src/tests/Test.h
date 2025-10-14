#pragma once
#include "../vendor/imgui/imgui.h"

namespace test {
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(const void* p, float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};
}