///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


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

    // 
    vec2 v2Texture = coreTexture2D(0, v_av2TexCoord[0]).rg * 2.0 - 1.0;

    // 
    vec2  v2Direction = normalize(v2Texture) * 0.5 + 0.5;
    float v1Length    = length   (v2Texture);

    // draw distortion (only two channels)
    gl_FragColor = vec4(v2Direction, 0.0, u_v4Color.a * v1Length * v1Alpha);
}