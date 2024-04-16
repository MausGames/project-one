///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
flat varying float v_v1Tickness;   // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPositionLow();

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordLow();
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;

    // 
    v_v1Tickness = 1.0 - (1.8 / u_v3Size.x);
}