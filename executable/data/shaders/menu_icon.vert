///////////////////////////////////////////////////////////
//*-----------------------------------------------------*//
//| Part of the Core Engine (https://www.maus-games.at) |//
//*-----------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics                 |//
//| Released under the zlib License                     |//
//*-----------------------------------------------------*//
///////////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


void VertexMain()
{
    // 
    gl_Position = coreObject2DPosition();

    // 
#if defined(_P1_SHADING_PIXEL_)
    v_av2TexCoord[0] = a_v2LowTexCoord;
#else
    v_av2TexCoord[0] = coreObject2DTexCoord();
#endif
}