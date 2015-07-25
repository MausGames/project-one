//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
uniform mat4 u_m4ShadowMatrix;    // own shadow view-projection matrix (with coordinate adjustment)

// shader output
varying vec4  v_v4ShadowCoord;    // pixel coordinates viewed from the light source
varying float v_v1Blink;          // base blink intensity (to highlight successful hits)


void VertexMain()
{
    // transform position and shadow projection
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    gl_Position        = u_m4ViewProj     * v4NewPosition;
    v_v4ShadowCoord    = u_m4ShadowMatrix * v4NewPosition;
    
    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
    v_av2TexCoord[1] = a_v2RawTexCoord * 1.2 + u_v2TexOffset;
    
    // forward blink intensity
    v_v1Blink = u_v2TexSize.x;

    // dot-3 transform lighting vectors
    coreDot3VertexInit();
    v_av4LightDir[0].xyz = coreDot3VertexTransform(-u_aLight[0].v4Direction.xyz);
    v_v3ViewDir          = coreDot3VertexTransform((u_m4Camera * -v4NewPosition).xyz);
}