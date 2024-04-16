///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec2 v_v2Tickness;   // 


void FragmentMain()
{
    // 
    float v1Detail1 = coreTexture2D(0, v_av2TexCoord[0]).r * 2.0 - 1.0/8.0;
    float v1Detail2 = coreTexture2D(1, v_av2TexCoord[1]).r * 2.0 - 1.0/8.0;
    vec2  v2Border  = smoothstep(v_v2Tickness - fwidth(v_av2TexCoord[2]), v_v2Tickness, abs(v_av2TexCoord[2]));

    // 
    float v1Intensity = coreLengthSq(v_av2TexCoord[2]) * 0.2;

    // 
    gl_FragColor = vec4(vec3((1.0 - 0.4 * max(v2Border.x, v2Border.y) - v1Intensity) * (0.5 + 0.5 * v1Detail1 + 0.7 * v1Detail2)) * mix(vec3(1.0), u_v4Color.rgb, u_v4Color.a), 0.7);
}