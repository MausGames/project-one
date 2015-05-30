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
#if defined(_P1_BULLET_)

    // transform position (and make flat)
    gl_Position = u_m4ViewProj * vec4(coreObject3DTransform(vec3(a_v3RawPosition.xy, 0.0)), 1.0);

    // orthographic view (for uniform shading)
    const vec3 v3ViewDir = vec3(0.0,0.0,1.0);
    
#else

    // transform view direction and position
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    vec3 v3ViewDir     = (u_m4Camera   * v4NewPosition).xyz;
    gl_Position        =  u_m4ViewProj * v4NewPosition;

    // perspective view (for default shading)
    v3ViewDir = normalize(-v3ViewDir);
    
#endif

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordRaw();

    // calculate light and color intensity
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = dot(v3ViewDir, vec3(v3NewNormal.xy, abs(v3NewNormal.z)));

#if defined(_P1_SPHERIC_)

    // spheric bubble-like
    v_v1Strength = (1.1 - v1Base) * 3.5;
    
#elif defined(_P1_INVERT_)

    // solid, stronger on the outside
    v_v1Strength = (1.4 - v1Base) * 3.5;

#elif defined(_P1_DIRECT_)

    // directional interpolated
    v1Base       =  1.0 - v1Base * a_v2RawTexCoord.y;
    v_v1Strength = (0.9 - v1Base) * 3.5;
    
    // increase depth on the back to improve overlapping
    gl_Position.z += (1.0-a_v2RawTexCoord.y) * gl_Position.w;

#else

    // solid, stronger on the inside
    v1Base       =  1.0 - v1Base;
    v_v1Strength = (0.8 - v1Base) * 3.5;

#endif
}