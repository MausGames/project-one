///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// constant values
const float c_v1TestFactor = 0.2;   // 

// shader input
varying float v_v1Mix;              // mix value between both outdoor textures
varying vec4  v_v4ShadowCoord;      // pixel coordinates viewed from the light source
varying vec2  v_v2Border;           // 


void FragmentMain()
{
    vec2 v2TexNormal;
    vec3 v3TexColor;

#if defined(_P1_LIGHT_)

    // ignore textures
    v2TexNormal = vec2(0.5);
    v3TexColor  = vec3(0.0);

#else

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

#endif

#if (_P1_SHADOW_) == 1 || defined(_P1_LIGHT_)

    // apply shadow mapping with single depth value comparison
    float v1Light = mix(1.0, 0.5, coreTextureBaseShadow(0, v_v4ShadowCoord));

    if(coreTextureBaseProj(3, v_v4ShadowCoord * vec4(vec2(c_v1TestFactor), 1.0, 1.0)).r != 0.0)
    {
        // 
        float v1DynLight = coreTextureBaseProj(3, v_v4ShadowCoord).r;

        // 
        float v1Visibility = coreSaturate(max(v_v2Border.x, v_v2Border.y));
        v1Light = mix(v1Light, 0.5, mix(v1DynLight, 0.0, v1Visibility));
    }

#elif (_P1_SHADOW_) >= 2

    const float A = 0.0012;

    // apply shadow mapping with percentage closer filtering
    float v1Light = (coreTextureBaseShadow(0, v_v4ShadowCoord)                            +
                     coreTextureBaseShadow(0, v_v4ShadowCoord + vec4(0.0,   A, 0.0, 0.0)) +
                     coreTextureBaseShadow(0, v_v4ShadowCoord + vec4(0.0,  -A, 0.0, 0.0)) +
                     coreTextureBaseShadow(0, v_v4ShadowCoord + vec4(  A, 0.0, 0.0, 0.0)) +
                     coreTextureBaseShadow(0, v_v4ShadowCoord + vec4( -A, 0.0, 0.0, 0.0))) * 0.2;
    v1Light = mix(1.0, 0.5, v1Light);

    if(coreTextureBaseProj(3, v_v4ShadowCoord * vec4(vec2(c_v1TestFactor), 1.0, 1.0)).r != 0.0)
    {
        // 
        float v1DynLight = (coreTextureBaseProj(3, v_v4ShadowCoord)                           .r +
                            coreTextureBaseProj(3, v_v4ShadowCoord + vec4(0.0,   A, 0.0, 0.0)).r +
                            coreTextureBaseProj(3, v_v4ShadowCoord + vec4(0.0,  -A, 0.0, 0.0)).r +
                            coreTextureBaseProj(3, v_v4ShadowCoord + vec4(  A, 0.0, 0.0, 0.0)).r +
                            coreTextureBaseProj(3, v_v4ShadowCoord + vec4( -A, 0.0, 0.0, 0.0)).r) * 0.2;

        // 
        float v1Visibility = coreSaturate(max(v_v2Border.x, v_v2Border.y));
        v1Light = mix(v1Light, 0.5, mix(v1DynLight, 0.0, v1Visibility));
    }

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
    float v1BumpFactor   = max(dot(v3MathLightDir, v3BumpNormal), v1Max);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(dot(v3ReflNormal, v3BumpNormal), 0.0);

    // calculate diffuse and specular value
    float v1Diffuse  = v1Light * (v1BumpFactor * 1.3 + 0.3);
    float v1Specular = 0.1 * min(coreGGX(v1ReflFactor, 0.09), 1.0) * v1Shine;

#if defined(_P1_LIGHT_)

    // 
    gl_FragColor = vec4((v1Diffuse + v1Specular) * (1.0 / 1.75), 1.0, 1.0, 1.0);

#else

    // draw final color
    gl_FragColor = vec4(v3TexColor * v1Diffuse + v1Specular, 1.0);

#endif
}