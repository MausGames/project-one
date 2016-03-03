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

uniform float u_v1Time;

// vertex attributes
attribute float a_v1Blink;        // 

// shader output
varying vec4 v_v4ShadowCoord;     // pixel coordinates viewed from the light source
varying vec4 v_av4ShipLight;      // simplified light vector (w = base blink intensity (to highlight successful hits))
varying vec3 v_av3ShipView;       // simplified view vector
varying vec3 v_av3ShipNormal;     // simplified ship normal


void VertexMain()
{

    vec3 v3WavePos = a_v3RawPosition;
#if defined(_P1_WAVE_) 
    v3WavePos.y += 0.12 * sin(v3WavePos.z*2.0 + u_v3Position.y * 0.3125 * PI - u_v1Time) * min(v3WavePos.z, 1.0);
#endif 

    // transform position and shadow projection
    vec4 v4NewPosition = vec4(coreObject3DTransform(v3WavePos), 1.0);
    
    vec4 v4NewPosition2 = vec4(coreObject3DTransformRaw(), 1.0);
    
    gl_Position        = u_m4ViewProj     * v4NewPosition;
    v_v4ShadowCoord    = u_m4ShadowMatrix * v4NewPosition2;
    
    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
    v_av2TexCoord[1] = coreObject3DTexCoordRaw();
    
    // forward blink intensity
#if defined(_CORE_OPTION_INSTANCING_)
    float v1Blink = a_v1Blink;
#else
    float v1Blink = u_v1Blink;
#endif

    // transform lighting properties
    coreLightingTransform(v4NewPosition.xyz);
    
    // 
    v_av4ShipLight  = vec4(-u_aLight[0].v4Direction.xyz, v1Blink);
    v_av3ShipView   = normalize(u_v3CamPosition - v4NewPosition.xyz);
    v_av3ShipNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
}