//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying float v_v1Strength;   // light and color intensity


void VertexMain()
{
    // transform view direction and position
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    vec3 v3ViewDir     = (u_m4Camera   * v4NewPosition).xyz;
    gl_Position        =  u_m4ViewProj * v4NewPosition;
    
    gl_Position.z = 0.0; // TODO #
    
    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordRaw();

    // calculate light and color intensity (stronger on the side)
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = dot(normalize(-v3ViewDir), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));

#if defined(_P1_SPHERIC_)

    v_v1Strength = (1.4 - v1Base) * 3.5;

#elif defined(_P1_DIRECT_)

    v1Base       =  1.0 - v1Base * a_v2RawTexCoord.y;
    v_v1Strength = (0.9 - v1Base) * 3.5;

#else

    v1Base       =  1.0 - v1Base;
    v_v1Strength = (0.8 - v1Base) * 3.5;

#endif
}