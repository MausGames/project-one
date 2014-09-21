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
        vec4 v4ViewDir = u_m4Camera * a_m4DivTransform * vec4(a_v3Position, 1.0);
        gl_Position    = u_m4Perspective * v4ViewDir;
        v_v4VarColor   = coreUnpackUnorm4x8(a_iDivColor);
    
    #ifdef _CORE_OPTION_NO_TEXTURE_TRANSFORM_
        v_av2TexCoord[0] = a_v2Texture;
    #else
        v_av2TexCoord[0] = a_v2Texture * a_v2DivTexSize + a_v2DivTexOffset;
    #endif
    
        coreDot3VertInit     (transpose(inverse(mat3(a_m4DivTransform))), a_v3Normal, a_v4Tangent);
        coreDot3VertTransform(-u_asLight[0].v4Direction.xyz, v_av4LightDir[0].xyz);
        coreDot3VertTransform(-v4ViewDir.xyz,                v_v3ViewDir);
    }
    
#else

    void main()
    {
        vec3 v3ViewDir = vec3(u_m4ModelView     * vec4(a_v3Position, 1.0));
        gl_Position    =      u_m4ModelViewProj * vec4(a_v3Position, 1.0);
        
    #ifdef _CORE_OPTION_NO_TEXTURE_TRANSFORM_
        v_av2TexCoord[0] = a_v2Texture;
    #else
        v_av2TexCoord[0] = a_v2Texture * u_v2TexSize + u_v2TexOffset;
    #endif
    
        coreDot3VertInit     (u_m3Normal, a_v3Normal, a_v4Tangent);
        coreDot3VertTransform(-u_asLight[0].v4Direction.xyz, v_av4LightDir[0].xyz);
        coreDot3VertTransform(-v3ViewDir,                    v_v3ViewDir);
    }
    
#endif
