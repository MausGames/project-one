///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec2 v_v2Tickness;   // 


void FragmentMain()
{
    // 
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r * 2.0 - 1.0/8.0;
    vec2  v2Border = smoothstep(v_v2Tickness - fwidth(v_av2TexCoord[1]), v_v2Tickness, abs(v_av2TexCoord[1]));

    // 
    gl_FragColor = vec4(vec3((1.0 - 0.4 * max(v2Border.x, v2Border.y)) * (0.5 + 0.5 * v1Detail)), 1.0) * u_v4Color;
}