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
    // transform position normal-resized (override depth to draw behind base object with depth-test)
    gl_Position   = u_m4ViewProj * vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition * u_v3Size) + (a_v3RawNormal * 0.22)) + u_v3Position, 1.0);
    gl_Position.z = 0.6 * gl_Position.w;

    // calculate outline intensity
    v_v1Strength = a_v2RawTexCoord.y * 2.0 - 1.0;
}