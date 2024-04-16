///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


#if defined(_P1_DIRECT_)

    // shader input
    varying float v_v1Strength;   // outline intensity

#else

    // constant values
    const float v_v1Strength = 0.8;

#endif


void FragmentMain()
{
    // 
#if defined(_P1_LIGHT_)
    const vec3 v3Base = vec3(0.9);
#else
    const vec3 v3Base = vec3(0.0);
#endif

    // draw semi-transparent outline
    gl_FragColor = vec4(v3Base, u_v4Color.a * v_v1Strength);
}