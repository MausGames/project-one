//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


#if defined(_P1_DIRECT_)

    // shader input
    varying float v_v1Strength;   // outline intensity

#else

    // constant values
    const float v_v1Strength = 0.65;

#endif


void FragmentMain()
{
    // draw semi-transparent outline
    gl_FragColor = vec4(0.1, 0.1, 0.1, u_v4Color.a * v_v1Strength);
}