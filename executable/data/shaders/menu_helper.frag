///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying float v_v1Threshold;   // 


void FragmentMain()
{
    // 
    float v1Width = fwidth(v_av2TexCoord[0].x) * 20.0;

    // 
    float v1Distance = coreTexture2D(0, v_av2TexCoord[0]).r;
    float v1Value    = coreSmoothStep(1.0           - v1Width, 1.0,           v1Distance);
    float v1Alpha    = coreSmoothStep(v_v1Threshold - v1Width, v_v1Threshold, v1Distance);

    // 
    float v1Intensity = 1.06 - coreLengthSq(v_av2TexCoord[1]) * 0.12;

    // 
    gl_FragColor = vec4(vec3(v1Value * v1Intensity), v1Alpha) * u_v4Color;
}