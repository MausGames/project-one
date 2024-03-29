///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
varying vec2 v_v2Move;   // 


void FragmentMain()
{
    // 
    vec2  v2Fract    = fract(v_v2Move);
    float v1Scanline = ((1.0 - smoothstep(0.25,       0.35,       v2Fract.x)) +
                        (1.0 - smoothstep(0.25 / 5.0, 0.35 / 5.0, v2Fract.y))) * 0.1;

    // 
    vec3 v3Detail = coreTexture2D(0, v_av2TexCoord[0]).rgb;

    // 
    gl_FragColor = vec4(mix(v3Detail * u_v4Color.rgb, vec3(c_v1White), v1Scanline), u_v4Color.a);
}