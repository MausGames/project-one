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
    // 
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord * 2.0 - 1.0;
    v_av2TexCoord[1] = u_v2TexOffset;
}