///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


// shader uniforms
uniform mediump vec3 u_av3OverlayTransform[_P1_OVERLAYS_];   // 

// shader output
smooth varying vec2 v_av2OverlayCoord[_P1_OVERLAYS_];        // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // 
    vec2 vBaseCoord = a_v2LowTexCoord * u_v2TexSize;

    // 
    for(int i = 0; i < (_P1_OVERLAYS_); ++i)
    {
        v_av2OverlayCoord[i] = vBaseCoord * u_av3OverlayTransform[i].z + u_av3OverlayTransform[i].xy;
    }
}