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
    // draw distortion map (only two channels)
    vec2 v2Distortion = coreTexture2D(0, v_av2TexCoord[0]).rg;
    gl_FragColor      = vec4(v2Distortion, 0.0, u_v4Color.a);
}