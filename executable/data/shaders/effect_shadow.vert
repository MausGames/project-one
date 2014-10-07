//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
uniform mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix


void VertexMain()
{
    // transform position viewed from the light source
    gl_Position = u_m4ShadowMatrix * (u_m4Transform * vec4(a_v3Position, 1.0));
}