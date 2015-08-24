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
    if(v_v1Mix <= 0.001)   // # performance boost
    {
        // lookup only lower outdoor textures
        v2TexNormal = coreTexture2D(2, v_av2TexCoord[0]).zw;
        v3TexColor  = coreTexture2D(1, v_av2TexCoord[0]).rgb;
    }
    else if(v_v1Mix >= 0.999)
    {
        // lookup only upper outdoor textures
        v2TexNormal = coreTexture2D(2, v_av2TexCoord[0]).xy;
        v3TexColor  = coreTexture2D(0, v_av2TexCoord[0]).rgb;
    }
    else
    {
        // lookup all outdoor textures
        vec4 v4FullNormal = coreTexture2D(2, v_av2TexCoord[0]);
        vec3 v3FullColor1 = coreTexture2D(1, v_av2TexCoord[0]).rgb;
        vec3 v3FullColor2 = coreTexture2D(0, v_av2TexCoord[0]).rgb;

        // mix between both layers
        v2TexNormal = mix(v4FullNormal.zw, v4FullNormal.xy, v_v1Mix);
        v3TexColor  = mix(v3FullColor1,    v3FullColor2,    v_v1Mix);
    }

#if defined(_P1_SHADOW_)

    // apply shadow mapping with depth value comparison
    float v1Light = 1.0 - coreTextureShadow(0, v_v4ShadowCoord) * 0.5;

#else

    // ignore shadow map
    const float v1Light = 1.0;

#endif

#if (_CORE_QUALITY_) == 0

    // ignore normal map
    v2TexNormal = vec2(0.5);

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
    vec3 v3Diffuse  = v3TexColor * (v1Light * (1.4 * max(0.0, v1BumpFactor) + 0.2));
    vec3 v3Specular = vec3(0.15 * pow(v1ReflFactor, 100.0));

    // draw final color
    gl_FragColor = vec4(v3Diffuse + v3Specular, 1.0);
}