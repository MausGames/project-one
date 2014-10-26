//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// transition uniforms
uniform float u_v1Transition;   // current transition time


void FragmentMain()
{
    // calculate transition line value for the wipe effect
    float v1Rev = 1.0 - v_av2TexCoord[0].y;
    float v1Mix = smoothstep(-0.05, 0.05, u_v1Transition - v1Rev);
    
    // draw only one texture or both
         if(v1Mix <= 0.001) gl_FragColor = vec4(coreTexture2D(0, v_av2TexCoord[0]).rgb, 1.0);
    else if(v1Mix >= 0.999) gl_FragColor = vec4(coreTexture2D(1, v_av2TexCoord[0]).rgb, 1.0);
    else
    {
        // lookup textures
        vec3 v3TexColorOld = coreTexture2D(0, v_av2TexCoord[0]).rgb;
        vec3 v3TexColorNew = coreTexture2D(1, v_av2TexCoord[0]).rgb;

        // interpolate smooth between both colors
        gl_FragColor = vec4(mix(v3TexColorOld, v3TexColorNew, v1Mix), 1.0);
    }
}