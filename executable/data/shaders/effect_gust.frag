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
    // draw only single channel (as alpha)
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(u_v4Color.rgb, u_v4Color.a * v1Value);
}