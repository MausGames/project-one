///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"
#include "engine/util_matrix.glsl"
#include "shared.glsl"


// constant values
const float c_v1TileRate = 1.4;     // 

// shader output
smooth varying vec4 v_v4Interior;   // raw object coordinates (xy = normal, zw = inverse)
flat   varying vec4 v_v4Border;     // border distance values (xy = inner, zw = outer)
smooth varying vec2 v_v2Factor;     // 


void VertexMain()
{
    // calculate non-uniform size-factor and offset
    vec2 v2Size   = AspectSize();
    vec2 v2Offset = u_v2TexOffset * 2.0;

    // 
    vec4 v4Pixel = u_v2TwoSize.xyxy;

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = (a_v2LowTexCoord * v2Size + v2Offset) * c_v1TileRate;

    // calculate border properties
    v_v4Interior = vec4(a_v2LowTexCoord, vec2(1.0) - a_v2LowTexCoord * u_v2TexSize) * v4Pixel + vec4(0.001);
    v_v4Border   = vec4(vec2(24.0/1080.0), vec2(14.0/1080.0)) / v2Size.xyxy;
    v_v4Border   = floor(v_v4Border * v4Pixel + vec4(0.001));

    // 
    float v1Max  = max(u_v4Resolution.x, u_v4Resolution.y);
    float v1Min  = min(u_v4Resolution.x, u_v4Resolution.y);
    float v1Term = (v1Max * 0.5 - v1Min * 0.4) / v4Pixel.y;
    v_v2Factor.x = (((a_v2LowTexCoord.y - (1.0 - v1Term)) / v1Term) * 1.1 - 0.1) * 2.2;
    v_v2Factor.y = (a_v2LowTexCoord.x - 0.5) * 0.8;
}