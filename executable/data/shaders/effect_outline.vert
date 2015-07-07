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
    gl_Position = u_m4ViewProj * vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition * u_v3Size) + (a_v3RawNormal * 0.31)) + u_v3Position, 1.0);

#if defined(_P1_DIRECT_)

    // increase depth on the back to improve overlapping
    gl_Position.z += (1.15-a_v2RawTexCoord.y) * gl_Position.w;

    // calculate interpolated outline intensity
    v_v1Strength = a_v2RawTexCoord.y * 2.0 - 1.0;
    
#else
    
    // increase depth to draw behind base object
    gl_Position.z += 0.4 * gl_Position.w;
    
#endif
}