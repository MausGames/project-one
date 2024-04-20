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
const float c_v1Size     = 31.0;   // number of vertices per axis
const float c_v1SizeHalf = 15.0;   // half number of vertices per axis (manually)
const float c_v1Detail   = 6.0;    // size of a block

// shader uniforms
uniform mat4 u_m4ShadowMatrix;     // own shadow view-projection matrix (with coordinate adjustment)
uniform vec4 u_v4LerpData1;        // 
uniform vec3 u_v3LerpData2;        // 

// vertex attributes
attribute float a_v1Height;        // vertex height (full position is deduced from the vertex-ID)
attribute vec2  a_v2Position;      // 

// shader output
varying vec4 v_v4ShadowCoord;      // pixel coordinates viewed from the light source
varying vec3 v_v3Border;           // (z = mix value between both outdoor textures) 


void LerpLightingTransform(const in vec3 v3Position, const in float v1Lerp)
{
    // 
    vec3 v3LerpNormal  = normalize(mix(vec3(0.0, 0.0, 1.0), a_v3RawNormal,      v1Lerp));
    vec3 v3LerpTangent = normalize(mix(vec3(1.0, 0.0, 0.0), a_v4RawTangent.xyz, v1Lerp));

    // 
    mat3 TBN = coreTangentSpaceMatrix(vec4(0.0), v3LerpNormal, vec4(v3LerpTangent, a_v4RawTangent.w));

    // 
    v_av4LightDir[0] = vec4(TBN * -u_av4LightDir[0].xyz, u_av4LightDir[0].w);
    v_v3TangentPos   = TBN * v3Position;
    v_v3TangentCam   = TBN * u_v3CamPosition;
}


void VertexMain()
{
#if defined(CORE_GL_gpu_shader4)

    // calculate vertex-position from vertex-ID (float-math for performance reasons)
    vec2 v2Index  = floor(vec2(mod(float(gl_VertexID), c_v1Size), float(gl_VertexID) / c_v1Size));
    vec2 v2Vertex = vec2(v2Index - vec2(c_v1SizeHalf)) * c_v1Detail;

#else

    // use vertex-position buffer (as fallback)
    vec2 v2Vertex = a_v2Position;

#endif

    vec2 v2MulAdd;

    // 
    float v1Lerp = floor((v2Vertex.y - u_v3CamPosition.y) / c_v1Detail) / c_v1Size + 0.5;
    if(v1Lerp <= u_v3LerpData2.z)
    {
        v2MulAdd = mix(u_v4LerpData1.xy, u_v3LerpData2.xy, v1Lerp / u_v3LerpData2.z);
    }
    else
    {
        v2MulAdd = mix(u_v3LerpData2.xy, u_v4LerpData1.zw, (v1Lerp - u_v3LerpData2.z) / (1.0 - u_v3LerpData2.z));
    }

    // transform position and shadow projection
    vec4 v4NewPosition = vec4(v2Vertex, a_v1Height * v2MulAdd.x + v2MulAdd.y, 1.0);
    gl_Position        = u_m4ViewProj     * v4NewPosition;
    v_v4ShadowCoord    = u_m4ShadowMatrix * v4NewPosition;

    // forward texture coordinates
    v_av2TexCoord[0] = v4NewPosition.xy * 0.03125;

    // 
    v_v3Border.xy = coreMat4to2(u_m4Camera) * (v4NewPosition.xy - u_v3CamPosition.xy) - v4NewPosition.z * vec2(-0.5, 0.5);
    v_v3Border.xy = v_v3Border.xy * vec2(-1.0, 1.0) * 0.132 - 5.5;

    // transform lighting properties (with interpolation)
    LerpLightingTransform(v4NewPosition.xyz, v2MulAdd.x);

    // calculate mix value relative to vertex height
    v_v3Border.z = v4NewPosition.z * 0.21 + 4.2;
}