///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject3DPositionLow();
    v_av2TexCoord[0] = coreObject3DTexCoordLow();
}