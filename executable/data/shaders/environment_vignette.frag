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
    // 
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
         v2ScreenCoord = abs(v2ScreenCoord * 2.0 - 1.0);

    // 
    vec3  v3Color = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    float v1Alpha = clamp(max(v2ScreenCoord.x, v2ScreenCoord.y) * 6.0 - 3.7, 0.0, 1.0);

    //if(v1Alpha == 1.0)
    //{
    //    gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
    //    return;
    //}
    //if(v1Alpha != 0.0)
    //{
    //    gl_FragColor = vec4(1.0, 0.0, 0.0, 0.5);
    //    return;
    //}

#if defined(_P1_GREY_)

    // 
    gl_FragColor = vec4(vec3(coreLuminance(v3Color)), v1Alpha);

#else

    // 
    gl_FragColor = vec4(v3Color, v1Alpha);

#endif
}