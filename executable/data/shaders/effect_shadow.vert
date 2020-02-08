///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix


void VertexMain()
{
    // transform position viewed from the light source
    gl_Position = u_m4ShadowMatrix * vec4(coreObject3DTransformRaw(), 1.0);
}