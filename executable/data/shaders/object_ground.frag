//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying vec4 v_v4ShadowCoord;   // pixel coordinates viewed from the light source


void FragmentMain()
{
    // lookup textures
    vec3 v3TexNormal = coreTexture2D(1, v_av2TexCoord[0]).xyz;
    vec3 v3TexColor  = coreTexture2D(0, v_av2TexCoord[0]).rgb;

#if defined(_P1_SHADOW_)

    // apply shadow mapping with depth value comparison
    float v1Light = 1.0 - coreTextureShadow(0, v_v4ShadowCoord) * 0.5;

#else

    // ingore shadow map
    const float v1Light = 1.0;

#endif

#if (_CORE_QUALITY_) == 0

    // ignore normal map
    v3TexNormal = vec3(0.5,0.5,1.0);

#endif

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize((2.0 * v1BumpFactor * v3BumpNormal) - v3MathLightDir);
    float v1ReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v3TexColor * (v1Light * (1.3 * max(0.0, v1BumpFactor) + 0.5));
    vec3 v3Specular = vec3(0.3 * 0.5 * pow(v1ReflFactor, 25.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * u_v4Color.rgb, u_v4Color.a);
}