///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform vec4 u_v4Position;   // 

// shader output
varying vec4 v_v4Diff;       // 


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = coreObject2DTexCoord();

    // 
    v_v4Diff = a_v2LowPosition.xyxy - u_v4Position;
}