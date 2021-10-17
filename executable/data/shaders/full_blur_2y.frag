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
    vec3 v3Color = vec3(0.0);

    // blur with a 5x5 Gaussian filter (Y axis)
    v3Color += (0.0545) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 * -2.0)).rgb;
    v3Color += (0.2442) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 * -1.0)).rgb;
    v3Color += (0.4026) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  0.0)).rgb;
    v3Color += (0.2442) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  1.0)).rgb;
    v3Color += (0.0545) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  2.0)).rgb;

    // blur with a 7x7 cone filter (Y axis)
    //v3Color += (1.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 * -3.0)).rgb;
    //v3Color += (2.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 * -2.0)).rgb;
    //v3Color += (3.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 * -1.0)).rgb;
    //v3Color += (4.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  0.0)).rgb;
    //v3Color += (3.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  1.0)).rgb;
    //v3Color += (2.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  2.0)).rgb;
    //v3Color += (1.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.0, 0.003 *  3.0)).rgb;

    gl_FragColor = vec4(coreSaturate(v3Color * u_v4Color.a), 1.0);
}