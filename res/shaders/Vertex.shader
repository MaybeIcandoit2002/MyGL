#version 330 core
layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_backgroundColor;
layout(location = 2) in vec2 in_texCoord;
layout(location = 3) in float in_TexIndex;
layout(std140) uniform u_TransForm
{
	mat3 u_positionTransform[128];
	// vec4 u_Color[128];
	// mat3 u_uvTransform[128];
};

out vec4 v_Color;
out vec2 v_texCoord;
out float v_TexIndex;
uniform mat4 u_proj;
uniform mat4 u_view;
void main()
{
	int idx = gl_InstanceID;
	gl_Position = u_proj * u_view * vec4((u_positionTransform[idx] * vec3(in_position.xy, 1.0)).xy, 0.0, 1.0);
	// v_Color = mix(in_backgroundColor, u_Color[idx], u_Color.a);
	// v_texCoord = (u_uvTransform[idx] * vec3(in_texCoord, 1.0)).xy;
	// v_TexIndex = in_TexIndex;
	v_Color = in_backgroundColor;
	v_texCoord = in_texCoord;
	v_TexIndex = in_TexIndex;
}