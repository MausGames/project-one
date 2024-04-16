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

    vec4 v4NewPosition  = vec4(coreObject3DTransformRaw(), 1.0);
    
    // 
    gl_Position      = u_m4ViewProj     * v4NewPosition;
    v_av2TexCoord[0] = coreObject3DTexCoordRaw();

    // transform lighting properties
    coreLightingTransform(v4NewPosition.xyz);
}