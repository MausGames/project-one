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
    vec2 v2TexCoord = v_av2TexCoord[0];

#if defined(_P1_DISTORTION_)

    // lookup distortion map
    vec2 v2Distortion = coreTexture2D(3, v_av2TexCoord[0]).rg;   // # low-res
    
    // move texture coordinates
    if(any(bvec4(lessThan(v2Distortion, vec2(127.0/255.0)), greaterThan(v2Distortion, vec2(128.0/255.0)))))
        v2TexCoord += (v2Distortion * 2.0 - 1.0) * vec2(-0.5,0.5);

#endif

    // lookup textures
    vec4 v4Foreground  = coreTexture2D(0, v2TexCoord);
    vec3 v3Environment = coreTexture2D(1, v2TexCoord).rgb;
    vec3 v3Glow        = coreTexture2D(2, v2TexCoord).rgb;   // # low-res
    
#if !defined(_P1_GLOW_)

    // ignore glow map
    v3Glow = vec3(0.0);

#endif

    // draw blend between all textures (glow only on environment for high contrast)
    gl_FragColor = vec4(mix(v3Environment + v3Glow, v4Foreground.rgb / v4Foreground.a, v4Foreground.a), 1.0);
}