///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"


// shader output
flat varying vec2 v_v2Tickness;   // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPositionLow();

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordLow();
    v_av2TexCoord[1] = a_v2LowTexCoord * u_v2TexSize + vec2(u_v2TexOffset.x, 0.0);
    v_av2TexCoord[2] = a_v2LowTexCoord * 2.0 - 1.0;

    // 
    v_v2Tickness = vec2(1.0) - (vec2(1.8) / u_v3Size.xy);
}