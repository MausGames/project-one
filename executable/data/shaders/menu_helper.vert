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


// shader output
flat varying float v_v1Threshold;   // 


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = coreObject2DTexCoord();
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;

    // 
    vec2 v2Size   = AspectSize();
    v_v1Threshold = 1.0 - 0.15 / (1.0 + length(v2Size));
}