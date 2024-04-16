///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec4  v_v4ShipLight;    // simplified light vector (w = base blink intensity (to highlight successful hits))
varying      vec3  v_v3ShipView;     // simplified view vector
varying      vec4  v_v4ShipNormal;   // simplified normal vector (w = depth darkening)


void FragmentMain()
{
    // lookup texture (rgb = base-color, a = highlight-modifier)
    vec4 v4TexColor = coreTexture2D(0, v_av2TexCoord[0]);

#if defined(_P1_DARKNESS_)

    // use detail texture as highlight
    float v1TexDetail = coreTexture2D(1, v_av2TexCoord[1]).r;
    vec3  v3Highlight = vec3(v1TexDetail * 8.0);

#else

    // use color as highlight
    vec3 v3Highlight = u_v4Color.rgb * 1.5;

#endif

#if defined(_P1_DETAIL_)

    // 
    float v1TexDetail = coreTexture2D(1, v_av2TexCoord[1]).r * 16.0 - 1.0;
    float v1Shift     = 1.0 + v1TexDetail * 0.2;

#else

    // 
    const float v1Shift = 1.0;

#endif

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_v4ShipLight.xyz);
    vec3  v3BumpNormal   = normalize(v_v4ShipNormal.xyz);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ShipView);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(dot(v3ReflNormal, v3BumpNormal), 0.0);

#if defined(_P1_GLOW_)

    // make highlighted area glowing
    v1BumpFactor = mix(v1BumpFactor, 1.0, v4TexColor.a * 0.5);

#endif

#if defined(_P1_BLINK_)

    // calculate smooth blink color
    vec3 v3Blink = vec3(v_v4ShipLight.w * (0.7 - 0.56 * abs(dot(v3MathViewDir, v3BumpNormal))));

#else

    // ignore blink color
    const vec3 v3Blink = vec3(0.0);

#endif

#if defined(_P1_DEPTH_)

    // 
    float v1Depth = v_v4ShipNormal.w;

#else

    // 
    const float v1Depth = 1.0;

#endif

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v4TexColor.rgb * max(v1BumpFactor * 1.2 + 0.6, 0.0) * v1Depth * v1Shift;
    vec3 v3Specular = vec3(0.15 * min(coreGGX(v1ReflFactor, 0.09), 1.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * mix(vec3(1.0), v3Highlight, v4TexColor.a) + v3Blink + vec3(coreDither() / 50.0), u_v4Color.a);
}