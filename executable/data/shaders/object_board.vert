///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"

// TODO 1: is object_board.vert still the same as object_tile.vert ?


// shader output
flat varying float v_v1Tickness;   // 
flat varying float v_v1Alpha;      // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPositionLow();

    // transform texture coordinates
    v_av2TexCoord[0] = a_v2LowTexCoord  + u_v2TexOffset;
    v_av2TexCoord[1] = a_v2LowTexCoord  * 2.0 - 1.0;
    v_av2TexCoord[2] = v_av2TexCoord[1] * u_v2TexSize;

    // 
    v_v1Tickness = 1.0 - (1.8 / u_v3Size.x);

    // 
    v_v1Alpha = coreSmoothStep(0.5, 1.0, u_v2TexSize.x);
}