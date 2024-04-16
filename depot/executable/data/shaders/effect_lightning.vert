///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


void VertexMain()
{
    // transform position (with zero depth)
    gl_Position   = coreObject3DPositionLow();
    gl_Position.z = 0.0;

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordLow();
}