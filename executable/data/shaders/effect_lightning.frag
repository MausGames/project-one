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
    // draw color with alpha from color channel
    vec3 v3Texture = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    gl_FragColor   = vec4(v3Texture, v3Texture.r) * u_v4Color;
}