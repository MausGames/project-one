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
const float c_v1TileRate = 2.0;     // 

// shader output
smooth varying vec2 v_v2Interior;   // raw object coordinates (x = normal, y = inverse)
flat   varying vec2 v_v2Border;     // border distance values (x = inner, y = outer)
flat   varying vec2 v_v2Scale;      // 


void VertexMain()
{
    // calculate non-uniform size-factor and offset
    vec2 v2Size   = AspectSize();
    vec2 v2Offset = u_v2TexOffset * 2.0;

    // 
    float v1Pixel = u_v2TwoSize.x;

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord * vec2(v2Size.x * 2.5, 1.0);
    v_av2TexCoord[1] = (a_v2LowTexCoord * v2Size + v2Offset) * c_v1TileRate;

    // calculate border properties
    v_v2Interior = vec2(a_v2LowTexCoord.x, 1.0 - a_v2LowTexCoord.x) * v1Pixel + vec2(0.001);
    v_v2Border   = vec2((24.0/1080.0), (14.0/1080.0)) / v2Size.x;
    v_v2Border   = floor(v_v2Border * v1Pixel + vec2(0.001));

    // 
    float v1Lerp = ((v2Size.y - 0.1) / v2Size.y) * 0.5;
    v_v2Scale    = vec2(0.5 - v1Lerp, 0.5 + v1Lerp);
}