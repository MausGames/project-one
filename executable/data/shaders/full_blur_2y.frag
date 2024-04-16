///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"


void FragmentMain()
{
    vec3  v3Color = vec3(0.0);
    float v1Step  = u_v4Resolution.w * 1.5;

    // blur with a 5x5 Gaussian filter (Y axis)
    v3Color += ( 7.0/107.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step * -2.0)).rgb;
    v3Color += (26.0/107.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step * -1.0)).rgb;
    v3Color += (41.0/107.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  0.0)).rgb;
    v3Color += (26.0/107.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  1.0)).rgb;
    v3Color += ( 7.0/107.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  2.0)).rgb;

    // blur with a 7x7 cone filter (Y axis)
    //v3Color += (1.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step * -3.0)).rgb;
    //v3Color += (2.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step * -2.0)).rgb;
    //v3Color += (3.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step * -1.0)).rgb;
    //v3Color += (4.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  0.0)).rgb;
    //v3Color += (3.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  1.0)).rgb;
    //v3Color += (2.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  2.0)).rgb;
    //v3Color += (1.0/16.0) * coreTextureBase2D(0, v_av2TexCoord[0] + vec2(0.0, v1Step *  3.0)).rgb;

    gl_FragColor = vec4(coreSaturate(v3Color * u_v4Color.a), 1.0);
}