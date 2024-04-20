///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord;
    v_av2TexCoord[1] = coreObject2DTexCoord();
}