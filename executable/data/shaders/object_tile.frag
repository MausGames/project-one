///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying float v_v1Tickness;   // 


void FragmentMain()
{
    // 
    float v1Width = fwidth(v_av2TexCoord[1].x);
    float v1Len   = length(v_av2TexCoord[2]);

    // 
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r * 2.0 - 1.0/8.0;
    float v1Border = smoothstep(v_v1Tickness - v1Width, v_v1Tickness, max(abs(v_av2TexCoord[1].x), abs(v_av2TexCoord[1].y)));
    float v1Shape  = smoothstep(0.5 - 3.0 * v1Width, 0.5, v1Len) - smoothstep(0.75 - 3.0 * v1Width, 0.75, v1Len);

    // 
    float v1Intensity = coreLengthSq(v_av2TexCoord[1]) * 0.2;

    // 
    gl_FragColor = vec4(vec3((1.0 - 0.85 * v1Border - v1Intensity) + v1Shape + v1Detail), 1.0) * u_v4Color;
}