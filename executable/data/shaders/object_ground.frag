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
varying vec4 v_v4ShadowCoord;       // pixel coordinates viewed from the light source
varying vec2 v_v2Border;            // 


void FragmentMain()
{
    // lookup textures
    vec3 v3TexNormal = coreTexture2D(1, v_av2TexCoord[1]).xyz;   // TODO 1: change (tex-coord?) back to 0, 1 is only needed for blocks, make own block shader with single-channel color
    vec3 v3TexColor  = coreTexture2D(0, v_av2TexCoord[1]).rgb;   // 

#if (_P1_SHADOW_) == 1

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

    // ingore shadow map
    const float v1Light = 1.0;

#endif

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float v1BumpFactor   = max(dot(v3MathLightDir, v3BumpNormal), 0.0);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(dot(v3ReflNormal, v3BumpNormal), 0.0);

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v3TexColor * (v1Light * (v1BumpFactor * 1.1 + 0.65));
    vec3 v3Specular = vec3(0.1 * min(coreGGX(v1ReflFactor, 0.09), 1.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * u_v4Color.rgb, 1.0);
}