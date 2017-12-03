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
    vec2 v2BaseCoord = a_v3RawPosition.xy + vec2(0.5);
    v_av2TexCoord[0] = v2BaseCoord;
    v_av2TexCoord[1] = v2BaseCoord * 3.0;         // for transition
    v_av2TexCoord[2] = v2BaseCoord * 2.0 - 1.0;   // for transition
}