///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
flat varying vec2 v_v2Tickness;   // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPositionLow();

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordLow();
    v_av2TexCoord[1] = a_v2LowTexCoord * 2.0 - 1.0;

    // 
    v_v2Tickness = vec2(1.0) - (vec2(1.8) / u_v3Size.xy);
}