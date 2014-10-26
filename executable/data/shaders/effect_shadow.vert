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
    // transform position viewed from the light source (with additional depth to reduce self-shadowing)
    gl_Position    = u_m4ShadowMatrix * vec4(coreObject3DTransform(), 1.0);
    gl_Position.z += 0.01;
}