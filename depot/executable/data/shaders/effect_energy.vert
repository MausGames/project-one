///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1Depth;        // 

// vertex attributes
attribute float a_v1Depth;      // 

// shader output
varying float v_v1Strength;     // light and color intensity
varying vec3  v_v3ShipView;     // simplified view vector
varying vec3  v_v3ShipNormal;   // simplified normal vector


void VertexMain()
{
#if defined(_P1_FLAT_)

    // transform position (and make flat)
    vec3 v3BasePosition = coreQuatApply(u_v4Rotation, a_v3RawPosition * u_v3Size);
    vec4 v4NewPosition  = vec4(v3BasePosition * vec3(1.0, 1.0, 0.0) + u_v3Position, 1.0);
    gl_Position         = u_m4ViewProj * v4NewPosition;

    // calculate light and color intensity
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = abs(v3NewNormal.z);

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
    gl_Position.z -= (v1Depth * 0.0012) * gl_Position.w;

#else

    // transform position
    vec4 v4NewPosition = vec4(coreObject3DTransformRaw(), 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // calculate light and color intensity
    vec3  v3ViewDir   = normalize(u_v3CamPosition - v4NewPosition.xyz);
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = dot(vec3(v3ViewDir.xy, abs(v3ViewDir.z)), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));

    // add slight z-offset to reduce fighting
    gl_Position.z -= 0.01 * gl_Position.w;

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

    // 
    v_v3ShipView   = u_v3CamPosition - v4NewPosition.xyz;   // # no normalization
    v_v3ShipNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
}