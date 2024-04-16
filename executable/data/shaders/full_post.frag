//////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
varying vec2 v_v2CenterCoord;   // 


void FragmentMain()
{
    vec4 v4Intermediate;

#if defined(_P1_CHROMA_)

    for(int i = 0; i < 3; ++i)
    {
    vec2 v2TexCoord = v_av2TexCoord[1] + vec2(u_v4Color.a * float(i - 1));

#else

    vec2 v2TexCoord = v_av2TexCoord[1];

#endif

#if defined(_P1_DISTORTION_)

    // lookup distortion map
    vec2 v2Distortion = coreTextureBase2D(3, v_av2TexCoord[1]).rg;   // # low-res

    // move texture coordinates
    #if (CORE_GL_VERSION >= 110) || (CORE_GL_ES_VERSION >= 300)
        const float A = 127.35;
        const float B = 127.65;
    #else
        const float A = 127.0;
        const float B = 128.0;
    #endif
    if(any(bvec4(lessThan(v2Distortion, vec2(A / 255.0)), greaterThan(v2Distortion, vec2(B / 255.0)))))
        v2TexCoord += (v2Distortion * 2.0 - 1.0) * vec2(-0.4, 0.4);

#endif

    // lookup textures
    vec4 v4Foreground  = coreTextureBase2D(0, v2TexCoord);
    vec3 v3Environment = coreTextureBase2D(1, v2TexCoord).rgb;
    vec3 v3Glow        = coreTextureBase2D(2, v2TexCoord).rgb;   // # low-res

#if !defined(_P1_GLOW_)

    // ignore glow map
    v3Glow = vec3(0.0);

#endif

    // vignetting 
    float v1Intensity = 1.0 - u_v4Color.b * coreLengthSq(v_v2CenterCoord);

    // draw blend between all textures (glow only on environment for high contrast)
    vec3 v3Blend = ((max((v3Environment - vec3(0.06)) * v1Intensity - vec3(coreLuminance(v3Glow) * 1.5), 0.0) + v3Glow) * (1.0 - v4Foreground.a) + v4Foreground.rgb * 1.05);

#if defined(_P1_TRANSPARENT_)
    float v1Alpha = max(coreLuminance(v3Environment), v4Foreground.a);
#else
    float v1Alpha = 1.0;
#endif

    // 
    vec3 v3Color = pow(v3Blend, vec3(1.05));
    vec3 v3Gray  = vec3(coreLuminance(v3Color));
    vec3 v3Final = mix(v3Gray, v3Color, u_v4Color.r);

#if defined(_P1_CHROMA_)

    v4Intermediate[i] = v3Final[i];
    v4Intermediate.a  = v1Alpha;
    }

#else

    v4Intermediate.rgb = v3Final;
    v4Intermediate.a   = v1Alpha;

#endif

    gl_FragColor.rgb = coreSaturate(v4Intermediate.rgb + vec3(coreDither() / 50.0));
    gl_FragColor.a   = v4Intermediate.a;

#if defined(_P1_DEBUG_)

    #define DEBUG_LINE(a,v,p) if((v_av2TexCoord[1].a > (v - 0.001)) && (v_av2TexCoord[1].a < (v + 0.001))) gl_FragColor.rgb = mix(vec3(0.0, 1.0, 0.0), gl_FragColor.rgb, p);

         DEBUG_LINE(x, 0.25,  0.6) else DEBUG_LINE(y, 0.25,  0.6)
    else DEBUG_LINE(x, 0.50,  0.6) else DEBUG_LINE(y, 0.50,  0.6)
    else DEBUG_LINE(x, 0.75,  0.6) else DEBUG_LINE(y, 0.75,  0.6)
    else DEBUG_LINE(x, 0.125, 0.8) else DEBUG_LINE(y, 0.125, 0.8)
    else DEBUG_LINE(x, 0.375, 0.8) else DEBUG_LINE(y, 0.375, 0.8)
    else DEBUG_LINE(x, 0.625, 0.8) else DEBUG_LINE(y, 0.625, 0.8)
    else DEBUG_LINE(x, 0.875, 0.8) else DEBUG_LINE(y, 0.875, 0.8)

#endif
}


//v3Environment.rgb = vec3(1.0 - coreLuminance(v3Environment.rgb));
//v3Environment.rgb = mix(v3Environment.rgb, vec3(coreLuminance(v3Environment.rgb)), u_v4Color.r);

//if(v3Gray.r != 0.0)
//{
//    vec3 v3Sepia = vec3(dot(v3Color, vec3(0.299, 0.587, 0.114))) + vec3(0.191, -0.054, -0.221);
//    gl_FragColor = vec4(mix(v3Color, v3Sepia, u_v4Color.r), 1.0);
//
//    //gl_FragColor = vec4(mix(v3Color, v3Gray, 0.7), 1.0);
//}
//gl_FragColor = vec4(mix(v3Color, v3Gray, step(6.0, dot(v3Color, vec3(1.0, 10.0, 10.0)))), 1.0);

//gl_FragColor = vec4(vec3(1.0 - v3Gray.x), 1.0);

//float v1Intensity = max(1.0 - mix(u_v4Color.b, u_v4Color.a, step(0.5, v_av2TexCoord[1].x)) * coreLengthSq(v_av2TexCoord[1] - vec2(0.5)), 0.0);