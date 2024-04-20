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


// constant values
const float c_v1TileRate = 2.0;   // 


void VertexMain()
{
    vec2 v2Aspect = u_v4Resolution.xy * u_v4Resolution.wz;

    // calculate non-uniform size-factor
    vec2 v2Size = abs(max(v2Aspect, 1.0) * coreMat3to2(u_m3ScreenView));

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord * u_v2TexSize;
    v_av2TexCoord[1] = a_v2LowPosition * 2.0;
    v_av2TexCoord[2] = (a_v2LowTexCoord * v2Size) * c_v1TileRate;
}