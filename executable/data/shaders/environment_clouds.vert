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
    // transform position (override depth for better early depth-test)
    gl_Position   = coreObject3DPositionLow();
    gl_Position.z = 0.25 * gl_Position.w;
    
    // transform texture coordinates
    v_av2TexCoord[0] = a_v2LowTexCoord + u_v2TexOffset;
    v_av2TexCoord[1] = a_v2LowTexCoord;
}