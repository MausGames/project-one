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


// constant values
const float c_v1MapResolution = 0.42;    // normal map resolution
const float c_v1WaterHeight   = -20.0;   // default water z-position

// shader uniforms
uniform highp float u_v1Offset;          // current y-position offset

// shader output
flat varying vec4 v_v4Lighting;          // lighting properties (xyz = light direction, w = height offset for smooth shores)


void VertexMain()
{
    // transform position (with slight z-offset to draw over water)
    gl_Position    = coreObject3DPositionLow();
    gl_Position.z -= 0.01 * gl_Position.w;

    // extract rotation from camera matrix
    mat3 m3Rotation = coreMat4to3(u_m4Camera);

    // transform lighting properties (resolved, with rotation for ray-tracing)
    v_v3TangentPos   = (m3Rotation * vec3(a_v2LowPosition, 0.0) * u_v3Size + u_v3Position) * vec3( 1.0, -1.0,  1.0);
    v_v3TangentCam   = (u_v3CamPosition)                                                   * vec3( 1.0, -1.0,  1.0);
    v_v4Lighting.xyz = (m3Rotation * u_av4LightDir[0].xyz)                                 * vec3(-1.0,  1.0, -1.0);

    // transform texture coordinates
    v_av2TexCoord[0] = vec2(a_v2LowTexCoord.x * c_v1MapResolution,
                            a_v2LowTexCoord.y * c_v1MapResolution + u_v1Offset) * 8.0;

    // calculate height offset
    v_v4Lighting.w = 0.042 * (u_v3Position.z - c_v1WaterHeight);
}