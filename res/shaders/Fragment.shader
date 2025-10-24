#version 330 core

in vec4 v_Color;
in vec2 v_texCoord;
in float v_TexIndex;

layout(location = 0) out vec4 color;
uniform sampler2D u_Textures[2];

vec4 SampleTextureByIndex(float idx)
{
    int texturIndex = int(idx);
    switch (texturIndex) {
        case 0:  return texture(u_Textures[0], v_texCoord);
        case 1:  return texture(u_Textures[1], v_texCoord);
        //case 2:  return texture(u_Textures[2], v_texCoord);
        //case 3:  return texture(u_Textures[3], v_texCoord);
        //case 4:  return texture(u_Textures[4], v_texCoord);
        //case 5:  return texture(u_Textures[5], v_texCoord);
        //case 6:  return texture(u_Textures[6], v_texCoord);
        //case 7:  return texture(u_Textures[7], v_texCoord);
        //case 8:  return texture(u_Textures[8], v_texCoord);
        //case 9:  return texture(u_Textures[9], v_texCoord);
        //case 10: return texture(u_Textures[10], v_texCoord);
        //case 11: return texture(u_Textures[11], v_texCoord);
        //case 12: return texture(u_Textures[12], v_texCoord);
        //case 13: return texture(u_Textures[13], v_texCoord);
        //case 14: return texture(u_Textures[14], v_texCoord);
        //case 15: return texture(u_Textures[15], v_texCoord);
        default: return vec4(1.0, 1.0, 1.0, 0.0);
    }
}

void main()
{
    vec4 texColor = SampleTextureByIndex(v_TexIndex);
    color = mix(v_Color, texColor, texColor.a);
}