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
    // draw texture with subtle white glow
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);
    gl_FragColor   = vec4(mix(vec3(1.0), v4Texture.rgb, v4Texture.a), v4Texture.a) * u_v4Color;
}