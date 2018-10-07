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
    // draw texture with grey-scale filter
    vec3 v3Color = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    gl_FragColor = vec4(vec3(coreLuminance(v3Color)) * u_v4Color.rgb, u_v4Color.a);
}