///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec2 v_v2Time;   // current simulation times (from 0.0 to 1.0)


void FragmentMain()
{
    // interpolate texture flipped over time
    float v1Value = mix(coreTexture2D(0, v_av2TexCoord[0]).r,
                        coreTexture2D(0, v_av2TexCoord[1]).r, v_v2Time.x);

    // draw as alpha map with decent highlights
    gl_FragColor = vec4(vec3(0.8 + 0.2 * v1Value) * u_v4Color.rgb, v1Value * (u_v4Color.a * u_v4Color.a) * v_v2Time.y);
}