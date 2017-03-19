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
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0) * vec4(1.0, 1.0, 0.0, 1.0);
    gl_Position = u_m4ViewProj * v4NewPosition;
    
    // 
    gl_Position.z -= (u_v3Size.z * 0.001) * gl_Position.w;

    // calculate light and color intensity
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = abs(v3NewNormal.z);

#else

    // transform position
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;
    
    // add slight z-offset to reduce fighting
    gl_Position.z -= 0.01 * gl_Position.w;

    // calculate light and color intensity
    vec3  v3ViewDir   = normalize(u_v3CamPosition - v4NewPosition.xyz);
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = dot(vec3(v3ViewDir.xy, abs(v3ViewDir.z)), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));

#endif

    // transform texture coordinates
    v_av2TexCoord[0] = coreObject3DTexCoordRaw();

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

#elif defined(_P1_RING_)

    // special-case with centric interpolation on y
    v_v1Strength = 3.5 * dot(normalize(a_v3RawPosition), a_v3RawNormal) * (1.0 - abs(a_v3RawNormal.y));

#else

    // solid, stronger on the inside
    v1Base       =  1.0 - v1Base;
    v_v1Strength = (0.8 - v1Base) * 3.5;

#endif
}