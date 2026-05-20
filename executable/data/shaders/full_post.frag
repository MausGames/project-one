///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"
#include "engine/util_color.glsl"


// shader input
smooth varying vec2 v_v2CenterCoord;   // 


void FragmentMain()
{
    vec4 v4Intermediate;

#if defined(_P1_CHROMA_) || defined(_P1_SHADING_SEPIA_)

    // 
    for(int i = 0; i < 3; ++i)
    {
    vec2 v2TexCoord = v_av2TexCoord[1] + vec2(u_v4Color.a * float(i - 1));

#else

    // 
    vec2 v2TexCoord = v_av2TexCoord[1];

#endif

#if defined(_P1_DISTORTION_)

    // 
    vec2 v2DistCoord = v_av2TexCoord[1];

    // lookup distortion map
    vec2 v2Distortion = coreTextureBase2D(3, v2DistCoord).rg;   // # low-res
         v2Distortion = v2Distortion * 2.0 - 1.0 + (1.0/255.0);

    // move texture coordinates
    v2TexCoord += v2Distortion * vec2(-0.1, 0.1);

#if defined(_P1_SHADING_SEPIA_)

    // 
    v2TexCoord += v2Distortion * vec2(u_v4Color.a * float(i - 1) * 100.0);

#endif

#endif

#if defined(_P1_SHADING_PIXEL_)

    // 
    vec2 v2Flip = u_v2TwoRotation * coreSign(u_v2TwoSize);
    mat2 m2Rota = mat2(v2Flip.y, -v2Flip.x, v2Flip.x, v2Flip.y);

    // 
    vec2 v2ResScale = vec2(min(u_v4Resolution.x, u_v4Resolution.y));
    vec2 v2ObjScale = vec2(u_v2TwoSize / min(abs(u_v2TwoSize.x), abs(u_v2TwoSize.y)));

    // 
    vec2 v2Factor    = m2Rota * (v2ObjScale * v2ResScale / round(v2ResScale / mix(50.0, 220.0, u_v4Color.r)));
    vec2 v2ForeCoord = (floor(v2TexCoord * v2Factor) + 0.5) / v2Factor;

#else

    // 
    vec2 v2ForeCoord = v2TexCoord;

#endif

    // lookup textures
    vec4 v4Foreground  = coreTextureBase2D(0, v2ForeCoord);
    vec3 v3Environment = coreTextureBase2D(1, v2TexCoord).rgb;
    vec3 v3Glow        = coreTextureBase2D(2, v2TexCoord).rgb;   // # low-res

#if defined(_P1_SHADING_PIXEL_)

    // 
    v4Foreground.rgb *= mix(1.1, 0.9, coreLengthSq(vec2(fract(v2TexCoord.x * v2Factor.x), 1.0 - fract(v2TexCoord.y * v2Factor.y))));
    v4Foreground.a    = coreSmoothBlend(v4Foreground.a);

#endif

#if defined(_P1_SHADING_RETRO_)

    // 
    v3Environment = vec3(mix(0.25, 0.04, u_v4Color.r));

#endif

#if defined(_P1_SHADING_SEPIA_)

    // 
    v3Environment *= mix(1.0, 0.85, u_v4Color.r);

#endif

#if !defined(_P1_GLOW_)

    // ignore glow map
    v3Glow = vec3(0.0);

#endif

    // apply vignetting effect
    float v1Intensity = 1.0 - u_v4Color.b * coreLengthSq(v_v2CenterCoord);

    // draw blend between all textures (glow only on environment for high contrast)
    vec3 v3Blend = ((max((v3Environment - vec3(0.06)) * v1Intensity - vec3(coreLuminance(v3Glow) * 1.5), 0.0) + v3Glow) * (1.0 - v4Foreground.a) + v4Foreground.rgb * 1.05);

    // 
#if defined(_P1_TRANSPARENT_)
    float v1Alpha = max(coreLuminance(v3Environment), v4Foreground.a);
#else
    const float v1Alpha = 1.0;
#endif

    // 
    vec3 v3Color = pow(max(v3Blend, vec3(0.0)), vec3(1.05));
    vec3 v3Gray  = vec3(coreLuminance(v3Color));
    vec3 v3Final = mix(v3Gray, v3Color, u_v4Color.r);

#if defined(_P1_SHADING_SEPIA_)

    // 
    vec3 v3Sepia = vec3(dot(v3Color, vec3(0.299, 0.587, 0.114))) + vec3(0.191, -0.054, -0.221);
    v3Final = mix(v3Color, v3Sepia, u_v4Color.r) * mix(1.0, v1Intensity, 0.5);

#endif

#if defined(_P1_CHROMA_) || defined(_P1_SHADING_SEPIA_)

    // 
    v4Intermediate[i] = v3Final[i];
    v4Intermediate.a  = v1Alpha;
    }

#else

    // 
    v4Intermediate.rgb = v3Final;
    v4Intermediate.a   = v1Alpha;

#endif

    // 
    gl_FragColor = vec4(coreSaturate(v4Intermediate.rgb * u_v4Color.g + vec3(coreDither() / 50.0)), v4Intermediate.a);

#if defined(_P1_DEBUG_)

    // 
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