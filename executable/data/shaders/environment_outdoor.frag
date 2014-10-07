//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_fMix;            // mix value between both outdoor textures
varying vec4  v_v4ShadowPixel;   // pixel coordinates viewed from the light source


void FragmentMain()
{ 
    vec3 v3TexNormal;
    vec4 v4TexColor;
    if(v_fMix <= 0.001)
    {
        // lookup only lower outdoor textures
        v3TexNormal = coreTexture2D(3, v_av2TexCoord[0]).xyz;
        v4TexColor  = coreTexture2D(2, v_av2TexCoord[0]);
    }
    else if(v_fMix >= 0.999)
    {
        // lookup only upper outdoor textures
        v3TexNormal = coreTexture2D(1, v_av2TexCoord[0]).xyz;
        v4TexColor  = coreTexture2D(0, v_av2TexCoord[0]);
    }
    else
    {
        // mix between both outdoor textures
        v3TexNormal = mix(coreTexture2D(3, v_av2TexCoord[0]).xyz, coreTexture2D(1, v_av2TexCoord[0]).xyz, v_fMix);
        v4TexColor  = mix(coreTexture2D(2, v_av2TexCoord[0]),     coreTexture2D(0, v_av2TexCoord[0]),     v_fMix);
    }
    
#if (_CORE_QUALITY_) == 0

    // ignore normal and specular map
    v3TexNormal  = vec3(0.5,0.5,1.0);
    v4TexColor.a = 0.5;
    
#endif
    
    // calculate dot-3 bump factor
    vec3 v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3 v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float fBumpFactor   = dot(v3MathLightDir, v3BumpNormal); 
    
    // calculate dot-3 reflection factor
    vec3 v3MathViewDir = normalize(v_v3ViewDir);
    vec3 v3ReflNormal  = normalize((2.0 * fBumpFactor * v3BumpNormal) - v3MathLightDir); 
    float fReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));

#if defined(_P1_SHADOW_)

    // apply shadow mapping with depth value comparison
    float fLight = 1.0 - coreTextureShadow(0, v_v4ShadowPixel) * 0.5;
    
#else
    const float fLight = 1.0;
#endif

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v4TexColor.rgb * (fLight * (1.3 * max(0.0, fBumpFactor) + 0.3));
    vec3 v3Specular = vec3(0.3 * v4TexColor.a * pow(fReflFactor, 25.0));
    
    // draw final color
    gl_FragColor = vec4(v3Diffuse + v3Specular, 1.0);
}