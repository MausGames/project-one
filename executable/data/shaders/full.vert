///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
varying vec2 v_v2CenterCoord;   // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // forward simplified texture coordinates
    vec2 v2BaseCoord = a_v2LowPosition + vec2(0.5);
    v_av2TexCoord[0] = v2BaseCoord;
    v_av2TexCoord[1] = v2BaseCoord * u_v2TexSize + u_v2TexOffset;
    v_av2TexCoord[2] = v2BaseCoord * 3.0;         // for transition
    v_av2TexCoord[3] = v2BaseCoord * 2.0 - 1.0;   // for transition

    // 
    v_v2CenterCoord = v_av2TexCoord[1] - vec2(0.5);
}