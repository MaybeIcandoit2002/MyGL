#version 330 core

in vec4 v_Color;
in vec2 v_texCoord;
in float v_TexIndex;

layout(location = 0) out vec4 color;
uniform sampler2D u_Textures[2];

void main()
{
    // 以纹理的 alpha 作为混合因子
    if(v_TexIndex <0)
        color = v_Color;
    else{
        int index = int(v_TexIndex);
        vec4 texColor = texture(u_Textures[index], v_texCoord);
        color = mix(v_Color, texColor, texColor.a);
    }
}