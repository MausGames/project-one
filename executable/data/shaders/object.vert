//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
uniform mat4 u_m4ShadowMatrix;   // own shadow view-projection matrix (with coordinate adjustment)

// shader output
varying vec4 v_v4ShadowPixel;   // pixel coordinates viewed from the light source


void VertexMain()
{
    // transform position and texture coordinates
    vec4 v4NewPosition = vec4(coreObject3DTransform(), 1.0);
    gl_Position        = u_m4ViewProj     * v4NewPosition;
    v_v4ShadowPixel    = u_m4ShadowMatrix * v4NewPosition;
    v_av2TexCoord[0]   = a_v2RawTexCoord;
    
    // dot-3 transform lighting vectors
    coreDot3VertexInit();
    v_av4LightDir[0].xyz = coreDot3VertexTransform(-u_aLight[0].v4Direction.xyz);
    v_v3ViewDir          = coreDot3VertexTransform((u_m4Camera * -v4NewPosition).xyz);
}