//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


#ifdef _CORE_OPTION_INSTANCING_

    void main()
    {
        gl_Position  = u_m4ViewProj * a_m4DivTransform * vec4(a_v3Position, 1.0);
        v_v4VarColor = coreUnpackUnorm4x8(a_iDivColor);
        
        v_av2TexCoord[0] = a_v2Texture + a_v2DivTexOffset;
        v_av2TexCoord[1] = a_v2Texture;
    }

#else

    void main()
    {
        gl_Position = u_m4ModelViewProj * vec4(a_v3Position, 1.0);
        
        v_av2TexCoord[0] = a_v2Texture + u_v2TexOffset;
        v_av2TexCoord[1] = a_v2Texture;
    }

#endif
