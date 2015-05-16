//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
#if defined(_P1_DIRECT_)

    // calculate burst-direction offset
    float v1Alpha = dot(normalize(v_av2TexCoord[1]), v_av2TexCoord[2]);
          v1Alpha = smoothstep(0.4, 0.9, v1Alpha);

#else

    // ignore offset
    const float v1Alpha = 1.0;

#endif

    // draw distortion map (only two channels)
    vec2 v2Distortion = coreTexture2D(0, v_av2TexCoord[0]).rg;
    gl_FragColor      = vec4(v2Distortion, 0.0, u_v4Color.a * v1Alpha);
}