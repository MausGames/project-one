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
    // lookup foreground map
    vec4 v4Foreground = coreTexture2D(0, v_av2TexCoord[0]);

    // draw only foreground on solid pixels
    if(v4Foreground.a >= 0.999) gl_FragColor = vec4(v4Foreground.rgb, 1.0);
    else
    {
        // lookup environment and glow map
        vec3 v3Environment = coreTexture2D(1, v_av2TexCoord[0]).rgb;
        vec3 v3Glow        = coreTexture2D(2, v_av2TexCoord[0]).rgb;

#if !defined(_P1_GLOW_)

        // ignore glow map
        v3Glow = vec3(0.0);

#endif

        // draw blend between all textures (glow only on environment for high contrast)
        gl_FragColor = vec4(mix(v3Environment + v3Glow, v4Foreground.rgb / v4Foreground.a, v4Foreground.a), 1.0);
    }
}