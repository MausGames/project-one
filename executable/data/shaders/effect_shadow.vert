///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"


// shader uniforms
uniform highp mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix


void VertexMain()
{
    // transform position viewed from the light source
    gl_Position = u_m4ShadowMatrix * vec4(coreObject3DTransformRaw(), 1.0);
}