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
    // transform position
    gl_Position = coreObject2DPosition();

    // transform texture coordinates
    v_av2TexCoord[0] = vec2(a_v2LowTexCoord.x, 1.0 - a_v2LowTexCoord.y);
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0;
    v_av2TexCoord[2] = a_v2LowTexCoord * 4.0;
    v_av2TexCoord[3] = a_v2LowTexCoord * 2.0 - 1.0;
}