//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void main()
{
    float fAlpha = 1.0 - coreLengthSq(2.0 * v_av2TexCoord[1] - 1.0);
    
    if(fAlpha > 0.0) 
    {
        float fValue = texture2D(u_as2Texture[0], v_av2TexCoord[0]).r;
        
#ifdef _CORE_OPTION_INSTANCING_ 
        gl_FragColor = vec4(vec3(0.4 + 0.6 * fValue), fAlpha * fValue * v_v4VarColor.a);
#else
        gl_FragColor = vec4(vec3(0.4 + 0.6 * fValue), fAlpha * fValue * u_v4Color.a);
        
#endif
    }
    else gl_FragColor = vec4(0.0);
}