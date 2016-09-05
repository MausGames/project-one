//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // transform texture coordinates
    v_av2TexCoord[0] = a_v2LowTexCoord * 2.0 - 1.0;
    v_av2TexCoord[1] = a_v2LowTexCoord;
}