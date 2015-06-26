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

    // forward simplified texture coordinates
    vec2 vNewTexCoord = a_v3RawPosition.xy + vec2(0.5);
    v_av2TexCoord[0]  = vNewTexCoord;
    v_av2TexCoord[1]  = vNewTexCoord * 3.0;   // for transition
}