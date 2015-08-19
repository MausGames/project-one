//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader uniforms
uniform mat4  u_m4ShadowMatrix;   // own shadow view-projection matrix (with coordinate adjustment)
uniform float u_v1Blink;          // 

// vertex attributes
attribute float a_v1Blink;        // 

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
    v_av2TexCoord[1] = coreObject3DTexCoordRaw();
    
    // forward blink intensity
#if defined(_CORE_OPTION_INSTANCING_)
    v_v1Blink = a_v1Blink;
#else
    v_v1Blink = u_v1Blink;
#endif

    // transform lighting properties
    coreLightingTransform(v4NewPosition.xyz);
    
    //v_v3TangentPos = v4NewPosition.xyz;
    //v_v3TangentCam = u_v3CamPosition;
    v_av4LightDir[1].xyz = -u_aLight[0].v4Direction.xyz;
    v_av4LightDir[2].xyz = normalize(u_v3CamPosition - v4NewPosition.xyz);
    v_av4LightDir[3].xyz = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    
}