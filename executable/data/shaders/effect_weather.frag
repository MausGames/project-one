///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// 
varying vec2 v_av2OverlayCoord[_P1_OVERLAYS_];   // 


vec4 GetMultipleTextures()
{
    vec4 v4Output = vec4(0.0);

    // 
    for(int i = 0; i < (_P1_OVERLAYS_); ++i)
    {
        v4Output += coreTexture2D(0, v_av2OverlayCoord[i]);
    }

    // 
    return min(v4Output, 1.0);
}

void FragmentMain()
{
#if defined(_P1_SINGLE_)

    // draw only single channel
    float v1Value = GetMultipleTextures().r;
    gl_FragColor  = vec4(u_v4Color.rgb, u_v4Color.a * v1Value);

#else

    // draw color with alpha
    vec4 v4Texture = GetMultipleTextures();
    gl_FragColor   = v4Texture * u_v4Color;

#endif
}