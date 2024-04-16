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
    // transform position
    gl_Position = coreObject3DPositionLow();

    // transform texture coordinates
    v_av2TexCoord[0] = a_v2LowTexCoord + u_v2TexOffset;
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;
}