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
    vec4  v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
    float v1Detail  = coreTexture2D(1, v_av2TexCoord[1]).r;

    // 
    gl_FragColor = vec4(v4Texture.rgb * (1.0 - v1Detail * 4.0) + v1Detail * 2.0, v4Texture.a) * u_v4Color;
}