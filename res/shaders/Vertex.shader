#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 u_Color;
layout(location = 2) in vec2 u_texCoord;
layout(location = 3) in float u_TexIndex;
layout(std140) uniform u_TransForm
{
	mat3 matScale;
	mat3 matRT;
};

out vec4 v_Color;
out vec2 v_texCoord;
out float v_TexIndex;
uniform mat4 u_proj;
uniform mat4 u_view;
void main()
{
	gl_Position = u_proj * u_view * vec4((matRT * matScale * vec3(position.xy, 1.0)).xy, 0.0, 1.0);
	v_Color = u_Color;
	v_texCoord = u_texCoord;
	v_TexIndex = u_TexIndex;
}