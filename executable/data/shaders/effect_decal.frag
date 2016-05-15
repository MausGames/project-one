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
#if defined(_P1_LIGHT_)

    // 
    vec2  v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
    float v1Light       = coreTexture2D(3, v2ScreenCoord).r + 0.04;
    
#else

    // 
    const float v1Light = 1.0;

#endif

#if defined(_P1_SPHERIC_)

    // calculate quartic distance from the center as alpha value
    float v1Alpha = coreLengthSq(v_av2TexCoord[1]);

    // only draw visible area
    if(v1Alpha < 1.0)
    {
        v1Alpha *= v1Alpha;
        v1Alpha *= v1Alpha;
        v1Alpha *= v1Alpha;
        v1Alpha  = 1.0 - v1Alpha;
        
        // draw color with alpha
        vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
        gl_FragColor   = vec4(v4Texture.rgb * v1Light, v4Texture.a * v1Alpha) * u_v4Color;
    }
    else discard; // gl_FragColor = vec4(0.0);

#elif defined(_P1_SINGLE_)

    // draw only single channel
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(u_v4Color.rgb * v1Light, u_v4Color.a * v1Value);

#else

    // draw color with alpha
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
    gl_FragColor   = vec4(v4Texture.rgb * v1Light, v4Texture.a) * u_v4Color;

#endif
}