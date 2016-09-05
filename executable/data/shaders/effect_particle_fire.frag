//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_v1Time;   // current animation time (from 0.0 to 1.0)


void FragmentMain()
{
    // interpolate texture flipped over time
    float v1Value = mix(coreTexture2D(0, v_av2TexCoord[0]).r, 
                        coreTexture2D(0, v_av2TexCoord[1]).r, min(v_v1Time * 3.0, 1.0)) * 1.2 * smoothstep(0.0, 0.3, u_v4Color.a);

    // draw as orange alpha map
    gl_FragColor = vec4(vec3(234.0/255.0, 72.0/255.0, 10.0/255.0), v1Value * (u_v4Color.a * u_v4Color.a));
}