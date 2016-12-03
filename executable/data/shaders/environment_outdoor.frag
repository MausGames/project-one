//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_v1Mix;           // mix value between both outdoor textures
varying vec4  v_v4ShadowCoord;   // pixel coordinates viewed from the light source


void FragmentMain()
{
    vec2 v2TexNormal;
    vec3 v3TexColor;

    // always lookup full normal map (less register pressure)
    vec4 v4FullNormal = coreTexture2D(2, v_av2TexCoord[0]);

    if(v_v1Mix <= 0.0)   // # performance boost
    {
        // lookup only lower outdoor textures
        v2TexNormal = v4FullNormal.zw;
        v3TexColor  = coreTexture2D(1, v_av2TexCoord[0]).rgb;
    }
    else if(v_v1Mix >= 1.0)
    {
        // lookup only upper outdoor textures
        v2TexNormal = v4FullNormal.xy;
        v3TexColor  = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    }
    else
    {
        // lookup all outdoor textures
        vec3 v3FullColor1 = coreTexture2D(1, v_av2TexCoord[0]).rgb;
        vec3 v3FullColor2 = coreTexture2D(0, v_av2TexCoord[0]).rgb;

        // mix between both layers
        v2TexNormal = mix(v4FullNormal.zw, v4FullNormal.xy, v_v1Mix);
        v3TexColor  = mix(v3FullColor1,    v3FullColor2,    v_v1Mix);
    }

#if (_CORE_QUALITY_) == 0 || defined(_P1_LIGHT_)

    // ignore normal map
    v2TexNormal = vec2(0.5);

#endif

#if (_P1_SHADOW_) == 1 || defined(_P1_LIGHT_)

    // apply shadow mapping with single depth value comparison
    float v1Light = 1.0 - coreTextureShadow(0, v_v4ShadowCoord) * 0.5;

#elif (_P1_SHADOW_) >= 2

    // apply shadow mapping with percentage closer filtering
    const float A = 0.0012;
    float v1Light = (coreTextureShadow(0, v_v4ShadowCoord)                            +
                     coreTextureShadow(0, v_v4ShadowCoord + vec4(0.0,   A, 0.0, 0.0)) +
                     coreTextureShadow(0, v_v4ShadowCoord + vec4(0.0,  -A, 0.0, 0.0)) +
                     coreTextureShadow(0, v_v4ShadowCoord + vec4( A,  0.0, 0.0, 0.0)) +
                     coreTextureShadow(0, v_v4ShadowCoord + vec4(-A,  0.0, 0.0, 0.0))) * 0.2;
    v1Light = 1.0 - v1Light * 0.5;

#else

    // ignore shadow map
    float v1Light = 1.0;

#endif

#if defined(_P1_GLOW_)

    // increase lower outdoor intensity (for volcano background)
    float v1Max = min(1.0 - v_v1Mix, 1.0);
    v1Light     = max(v1Max, v1Light);

    // 
    const float v1Shine = 0.0;

#else

    // 
    const float v1Max   = 0.0;
    const float v1Shine = 1.0;

#endif

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = coreUnpackNormalMapDeriv(v2TexNormal);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(0.0, dot(v3ReflNormal, v3BumpNormal));

    // calculate diffuse and specular value
    float v1Diffuse  = v1Light * (1.4 * max(v1Max, v1BumpFactor) + 0.2);
    float v1Specular = 0.15 * pow(v1ReflFactor, 100.0) * v1Shine;

#if defined(_P1_LIGHT_)

    // 
    gl_FragColor = vec4(v1Diffuse + v1Specular, 1.0, 1.0, 1.0);

#else

    // draw final color
    gl_FragColor = vec4(v3TexColor * v1Diffuse + v1Specular, 1.0);

#endif
}