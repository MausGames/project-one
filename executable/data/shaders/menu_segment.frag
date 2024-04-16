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
    // 
    float v1Intensity = 1.03 - coreLengthSq(v_av2TexCoord[1]) * 0.06;

    // draw only single channel (as color)
    float v1Value = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor  = vec4(u_v4Color.rgb * (v1Value * v1Intensity), u_v4Color.a);
}