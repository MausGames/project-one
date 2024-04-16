///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"

#if defined(_P1_OBJECT3D_)
    #include "engine/data_transform_3d.glsl"
#endif


void VertexMain()
{
    // transform position
#if defined(_P1_OBJECT3D_)
    gl_Position = u_m4Ortho * vec4(coreObject3DTransformLow(), 1.0);
#else
    gl_Position = coreObject2DPosition();
#endif

    // transform texture coordinates and extract burst-direction
    v_av2TexCoord[0] = a_v2LowTexCoord;
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;
    v_av2TexCoord[2] = normalize(u_v4Color.gb * vec2(1.0, -1.0));
}