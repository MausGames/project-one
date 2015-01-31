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
    vec4 v4Foreground = coreTexture2D(1, v_av2TexCoord[0]);
    
    if(v4Foreground.a == 1.0) gl_FragColor = vec4(v4Foreground.rgb, 1.0);
    else
    {
        vec3 v3Environment = coreTexture2D(0, v_av2TexCoord[0]).rgb;
        vec3 v3Glow        = coreTexture2D(2, v_av2TexCoord[0]).rgb;
        gl_FragColor = vec4(mix(v3Environment + v3Glow.rgb, v4Foreground.rgb / v4Foreground.a, v4Foreground.a), 1.0);
    }
}