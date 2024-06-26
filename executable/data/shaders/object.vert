///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"
#include "engine/data_ambient.glsl"


// shader uniforms
uniform mat4  u_m4ShadowMatrix;       // own shadow view-projection matrix (with coordinate adjustment)
uniform float u_v1Blink;              // 
uniform float u_v1Time;               // 

// vertex attributes
attribute float a_v1Blink;            // 

// shader output
varying      vec4  v_v4ShadowCoord;   // pixel coordinates viewed from the light source
varying      vec2  v_v2Border;        // 
flat varying vec4  v_v4ShipLight;     // simplified light vector (w = base blink intensity (to highlight successful hits))
varying      vec3  v_v3ShipView;      // simplified view vector
varying      vec4  v_v4ShipNormal;    // simplified normal vector (w = depth darkening)


void VertexMain()
{
#if defined(_P1_WAVE_)

    // 
    vec3 v3WavePos    = a_v3RawPosition;
         v3WavePos.y += 0.22 * sin(v3WavePos.z * 2.0 + u_v3Position.y * 0.3125 * PI - u_v1Time) * clamp(v3WavePos.z * 1.0 - 1.0, 0.0, 1.0);

    // 
    vec4 v4NewPosition  = vec4(coreObject3DTransform(v3WavePos), 1.0);
    vec4 v4NewPosition2 = vec4(coreObject3DTransformRaw(),       1.0);

#else

    // 
    vec4 v4NewPosition  = vec4(coreObject3DTransformRaw(), 1.0);
    vec4 v4NewPosition2 = v4NewPosition;

#endif

    // transform position and shadow projection
    gl_Position     = u_m4ViewProj     * v4NewPosition;
    v_v4ShadowCoord = u_m4ShadowMatrix * v4NewPosition2;

    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
    v_av2TexCoord[1] = coreObject3DTexCoordRaw();

    // 
    v_v2Border = coreMat4to2(u_m4Camera) * (v4NewPosition.xy - u_v3CamPosition.xy) - v4NewPosition.z * vec2(-0.5, 0.5);
    v_v2Border = v_v2Border * vec2(-1.0, 1.0) * 0.132 - 5.5;

    // select blink intensity
#if defined(_CORE_OPTION_INSTANCING_)
    float v1Blink = a_v1Blink;
#else
    float v1Blink = u_v1Blink;   // TODO 1: put into fragment shader
#endif

    // transform lighting properties
    coreLightingTransform(v4NewPosition.xyz);

    // 
    v_v4ShipLight      = vec4(-u_av4LightDir[0].xyz, v1Blink);
    v_v3ShipView       = u_v3CamPosition - v4NewPosition.xyz;   // # no normalization
    v_v4ShipNormal.xyz = coreQuatApply(u_v4Rotation, a_v3RawNormal);

    // 
    v_v4ShipNormal.w = v4NewPosition.z * 0.1 + 0.8;
}