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
    // draw gray texture with highlights (r = texture, g = highlight)
    vec2 v2Value = coreTexture2D(0, v_av2TexCoord[0]).rg;
    gl_FragColor = vec4(vec3(0.5 * v2Value.r + (1.0-v2Value.g)), 1.0) * u_v4Color.a;
}