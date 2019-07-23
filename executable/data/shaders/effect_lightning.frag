///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


void FragmentMain()
{
    // draw color with alpha from color channel
    vec3 v3Texture = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    gl_FragColor   = vec4(v3Texture, v3Texture.r) * u_v4Color;
}