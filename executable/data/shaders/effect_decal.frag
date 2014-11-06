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
#if defined(_P1_SPHERIC_)

    // calculate quartic distance from the center as alpha value
    float v1Alpha = coreLengthSq(2.0 * v_av2TexCoord[1] - 1.0);
          v1Alpha = 1.0 - v1Alpha * v1Alpha;
    
    // only draw visible area
    if(v1Alpha > 0.0) 
    {
        // draw color with alpha
        vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
        gl_FragColor  = vec4(v4Texture.rgb, v4Texture.a * v1Alpha) * u_v4Color;
    }
    else gl_FragColor = vec4(0.0);
    
#else

    // draw color with alpha
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
    gl_FragColor   = v4Texture * u_v4Color;
    
#endif
}