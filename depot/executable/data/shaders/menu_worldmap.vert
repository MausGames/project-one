///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1Time;   // 

// shader output
varying vec2 v_v2Move;    // 


void VertexMain()
{
    // 
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = coreObject2DTexCoord();

    // 
    v_v2Move = vec2((v_av2TexCoord[0].y * 30.0 - u_v1Time),
                    (v_av2TexCoord[0].y * 30.0 - u_v1Time * 2.0) / 5.0);
}