///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
flat varying vec4 v_av4ShipLight;     // simplified light vector (w = base blink intensity (to highlight successful hits))
varying      vec3 v_av3ShipView;      // simplified view vector
varying      vec3 v_av3ShipNormal;    // simplified normal vector


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

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4ShipLight.xyz);
    vec3  v3BumpNormal   = normalize(v_av3ShipNormal);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_av3ShipView);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(0.0, dot(v3ReflNormal, v3BumpNormal));

#if defined(_P1_GLOW_)

    // make highlighted area glowing
    v1BumpFactor += v4TexColor.a * 0.5;

#endif

#if defined(_P1_BLINK_)

    // calculate smooth blink color
    vec3 v3Blink = vec3(v_av4ShipLight.w * (0.7 - 0.56 * abs(dot(v3MathViewDir, v3BumpNormal))));

#else

    // ignore blink color
    const vec3 v3Blink = vec3(0.0);

#endif

    // calculate diffuse and specular value
    vec3 v3Diffuse  = v4TexColor.rgb * (1.4 * max(0.0, v1BumpFactor) + 0.4);
    vec3 v3Specular = vec3(0.2 * pow(v1ReflFactor, 100.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * mix(vec3(1.0), v3Highlight, v4TexColor.a) + v3Blink, u_v4Color.a);
}