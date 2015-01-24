//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying float v_v1Light;   // light and color intensity


void VertexMain()
{
    // transform view direction and position
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    vec3 v3ViewDir     = (u_m4Camera   * v4NewPosition).xyz;
    gl_Position        =  u_m4ViewProj * v4NewPosition;
    
    // calculate light and color intensity (stronger on the side)
    v_v1Light = dot(normalize(-v3ViewDir), coreQuatApply(u_v4Rotation, a_v3RawNormal));
	v_v1Light = (1.1 - v_v1Light) * 2.5;
}