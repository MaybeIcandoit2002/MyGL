#version 330 core

in vec4 v_Color;
in vec2 v_texCoord;
in float v_TexIndex;

layout(location = 0) out vec4 color;
uniform sampler2D u_Textures[2];

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_texCoord);
    // 以纹理的 alpha 作为混合因子
    color = mix(v_Color, texColor, texColor.a);
}