///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1Blink;        // 

// shader input
varying float v_v1Strength;     // light and color intensity
varying vec3  v_v3ShipView;     // simplified view vector
varying vec3  v_v3ShipNormal;   // simplified normal vector


void FragmentMain()
{
#if (_CORE_QUALITY_ >= 1)

    // evaluate shader per sample
    CORE_SAMPLE_SHADING

#endif

#if defined(_P1_BLINK_)

    // 
    vec3 v3MathViewDir = normalize(v_v3ShipView);
    vec3 v3BumpNormal  = normalize(v_v3ShipNormal);

    // calculate smooth blink color
    vec3 v3Blink = vec3(u_v1Blink * (0.7 - 0.56 * abs(dot(v3MathViewDir, v3BumpNormal))));

#else

    // ignore blink color
    const vec3 v3Blink = vec3(0.0);

#endif

    // lookup detail value and map between -1.0 and +1.0
    float v1Detail = coreTexture2D(0, v_av2TexCoord[0]).r;
          v1Detail = v1Detail * 2.0 - 1.0;

    // draw blended color
    float v1Full = max(v_v1Strength + v1Detail, 0.0);
    gl_FragColor = vec4(u_v4Color.rgb * v1Full + v3Blink, u_v4Color.a * min(v1Full, 1.0));
}