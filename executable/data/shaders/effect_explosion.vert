///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"


// shader output
smooth varying float v_v1Light;   // 


void VertexMain()
{
    // 
    gl_Position = coreObject3DPositionRaw();

    // 
    v_v1Light = pow(abs(a_v3RawNormal.z), 10.0) * 5.0 + 2.6;
}