///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1Depth;      // 

// vertex attributes
attribute float a_v1Depth;    // 

// shader output
varying float v_v1Strength;   // outline intensity


void VertexMain()
{
    // transform position normal-resized
    vec3 v3BasePosition = coreQuatApply(u_v4Rotation, (a_v3RawPosition * u_v3Size) + (a_v3RawNormal * 0.31));

#if defined(_P1_FLAT_)

    // (and make flat) 
    gl_Position = u_m4ViewProj * vec4(v3BasePosition * vec3(1.0, 1.0, 0.0) + u_v3Position, 1.0);

    // 
#if defined(_P1_BULLET_)
    #if defined(_CORE_OPTION_INSTANCING_)
        float v1Depth = a_v1Depth;
    #else
        float v1Depth = u_v1Depth;
    #endif
#else
    const float v1Depth = 0.0;
#endif

    // 
    gl_Position.z -= (v1Depth * 0.001 - 0.0005) * gl_Position.w;

#else

    // 
    gl_Position = u_m4ViewProj * vec4(v3BasePosition + u_v3Position, 1.0);

    // increase depth to draw behind base object  
    gl_Position.z += 0.4 * gl_Position.w;

#endif

#if defined(_P1_DIRECT_)

    // calculate interpolated outline intensity
    v_v1Strength = a_v2RawTexCoord.y * 2.0 - 1.0;

#endif
}