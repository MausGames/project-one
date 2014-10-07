//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// transition uniforms
uniform float u_fTransition;   // current transition time


void FragmentMain()
{
    // calculate transition line value for the wipe effect
    float fRev = 1.0 - v_av2TexCoord[0].y;
    float fMix = smoothstep(-0.05, 0.05, u_fTransition - fRev);
    
    // draw only one texture or both
         if(fMix <= 0.001) gl_FragColor = vec4(coreTexture2D(0, v_av2TexCoord[0]).rgb, 1.0);
    else if(fMix >= 0.999) gl_FragColor = vec4(coreTexture2D(1, v_av2TexCoord[0]).rgb, 1.0);
    else
    {
        // lookup textures
        vec3 v3TexColorOld = coreTexture2D(0, v_av2TexCoord[0]).rgb;
        vec3 v3TexColorNew = coreTexture2D(1, v_av2TexCoord[0]).rgb;

        // interpolate smooth between both colors
        gl_FragColor = vec4(mix(v3TexColorOld, v3TexColorNew, fMix), 1.0);
    }
}