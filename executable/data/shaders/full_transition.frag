//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// transition time uniform
uniform float u_fTransition;


void main()
{
    float fRev = 1.0 - v_av2TexCoord[0].y;
    float fMix = smoothstep(-0.05, 0.05, u_fTransition - fRev);
    
         if(fMix <= 0.0) gl_FragColor = vec4(texture2D(u_as2Texture[0], v_av2TexCoord[0]).rgb, 1.0);
    else if(fMix >= 1.0) gl_FragColor = vec4(texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb, 1.0);
    else
    {
        vec3 vColorOld = texture2D(u_as2Texture[0], v_av2TexCoord[0]).rgb;
        vec3 vColorNew = texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb;

        gl_FragColor = vec4(mix(vColorOld, vColorNew, fMix), 1.0);
    }
}