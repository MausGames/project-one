//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying float v_v1Strength;   // outline intensity


void VertexMain()
{
    // transform position normal-resized
    vec4 v4NewPosition = vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition * u_v3Size) + (a_v3RawNormal * 0.31)) + u_v3Position, 1.0);

#if defined(_P1_BULLET_)

    // (and make flat) 
    v4NewPosition *= vec4(1.0, 1.0, 0.0, 1.0);
    gl_Position    = u_m4ViewProj * v4NewPosition;

    // 
    gl_Position.z -= (u_v3Size.z * 0.001 - 0.0005) * gl_Position.w;

#else

    // 
    gl_Position = u_m4ViewProj * v4NewPosition;

    // increase depth to draw behind base object  
    gl_Position.z += 0.4 * gl_Position.w;

#endif

#if defined(_P1_DIRECT_)

    // calculate interpolated outline intensity
    v_v1Strength = a_v2RawTexCoord.y * 2.0 - 1.0;

#endif
}