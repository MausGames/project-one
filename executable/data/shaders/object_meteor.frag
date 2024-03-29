///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec4 v_v4ShipLight;   // simplified light vector (w = base blink intensity (to highlight successful hits))


void FragmentMain()
{
    // lookup textures
    vec3 v3TexNormal = coreTexture2D(1, v_av2TexCoord[0]).xyz;
    vec3 v3TexColor  = coreTexture2D(0, v_av2TexCoord[0]).rgb;

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float v1BumpFactor   = max(dot(v3MathLightDir, v3BumpNormal), 0.0);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(dot(v3ReflNormal, v3BumpNormal), 0.0);

#if defined(_P1_BLINK_)

    // calculate smooth blink color
    vec3 v3Blink = vec3(v_v4ShipLight.w * (0.7 - 0.56 * abs(dot(v3MathViewDir, v3BumpNormal))));

#else

    // ignore blink color
    const vec3 v3Blink = vec3(0.0);

#endif

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v3TexColor * (v1BumpFactor * 1.1 + 0.65);
    vec3 v3Specular = vec3(0.1 * min(coreGGX(v1ReflFactor, 0.09), 1.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * u_v4Color.rgb * 1.5 + v3Blink, u_v4Color.a);
}