#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace RenderComponents {
	struct Vertexs2D;
	struct TransForms2D;

	class Renderer;
	class ShaderProgram;
}

struct RenderComponents::Vertexs2D
{
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float texID;
};
struct RenderComponents::TransForms2D {
	glm::mat3x4 matSRT;
};