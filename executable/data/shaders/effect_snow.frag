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
    // 
    float v1Value  = coreTexture2D(0, v_av2TexCoord[0]).r;
    float v1Detail = coreTexture2D(1, v_av2TexCoord[1]).r;
    float v1Offset = coreTexture2D(1, v_av2TexCoord[2]).r * 16.0 - 1.0;

    // 
    float v1Mix = smoothstep(-0.22 + 0.5, 0.22 + 0.5, v1Value + v1Offset * 0.3);

    // 
    gl_FragColor = vec4(vec3((1.0 - v1Detail) * mix(0.5, 1.0, v1Mix)), v1Mix * u_v4Color.a);
}