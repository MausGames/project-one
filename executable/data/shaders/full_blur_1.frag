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
    vec3 v3Color = vec3(0.0);

    // blur with a 5x5 box filter (X axis)
    //v3Color += (1.0/9.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 * -2.0, 0.0)).rgb;
    //v3Color += (2.0/9.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 * -1.0, 0.0)).rgb;
    //v3Color += (3.0/9.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  0.0, 0.0)).rgb;
    //v3Color += (2.0/9.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  1.0, 0.0)).rgb;
    //v3Color += (1.0/9.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  2.0, 0.0)).rgb;

    // blur with a 7x7 box filter (X axis)
    v3Color += (1.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 * -3.0, 0.0)).rgb;
    v3Color += (2.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 * -2.0, 0.0)).rgb;
    v3Color += (3.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 * -1.0, 0.0)).rgb;
    v3Color += (4.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  0.0, 0.0)).rgb;
    v3Color += (3.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  1.0, 0.0)).rgb;
    v3Color += (2.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  2.0, 0.0)).rgb;
    v3Color += (1.0/16.0) * coreTexture2D(0, v_av2TexCoord[0] + vec2(0.003 *  3.0, 0.0)).rgb;

    gl_FragColor = vec4(v3Color, 1.0);
}