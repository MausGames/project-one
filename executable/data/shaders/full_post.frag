//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
    vec2 v2TexCoord = v_av2TexCoord[0];

#if defined(_P1_DISTORTION_)

    // lookup distortion map
    vec2 v2Distortion = coreTexture2D(3, v_av2TexCoord[0]).rg;   // # low-res
    
    // move texture coordinates
    if(any(bvec4(lessThan(v2Distortion, vec2(127.4/255.0)), greaterThan(v2Distortion, vec2(127.6/255.0)))))
        v2TexCoord += (v2Distortion * 2.0 - 1.0) * vec2(-0.5,0.5);

#endif

    // lookup textures
    vec4 v4Foreground  = coreTexture2D(0, v2TexCoord);
    vec3 v3Environment = coreTexture2D(1, v2TexCoord).rgb;
    vec3 v3Glow        = coreTexture2D(2, v2TexCoord).rgb;   // # low-res
    
#if !defined(_P1_GLOW_)

    // ignore glow map
    v3Glow = vec3(0.0);

#endif

    // draw blend between all textures (glow only on environment for high contrast)
    vec3 v3Blend = mix(v3Environment * u_v4Color.g + v3Glow, v4Foreground.rgb / max(v4Foreground.a, 0.001), v4Foreground.a);
    
    // 
    vec3 v3Color = pow(v3Blend, vec3(1.05));
    vec3 v3Gray  = vec3(dot(v3Color, vec3(0.212671, 0.715160, 0.072169)));
    gl_FragColor = vec4(mix(v3Color, v3Gray, u_v4Color.r), 1.0);
    
    //if(v3Gray.r != 0.0)
    //{
    //    vec3 v3Sepia = vec3(dot(v3Color, vec3(0.299, 0.587, 0.114))) + vec3(0.191, -0.054, -0.221);
    //    gl_FragColor = vec4(mix(v3Color, v3Sepia, u_v4Color.r), 1.0);
    //    
    //    //gl_FragColor = vec4(mix(v3Color, v3Gray, 0.7), 1.0);
    //}
    //gl_FragColor = vec4(mix(v3Color, v3Gray, step(6.0, dot(v3Color, vec3(1.0, 10.0, 10.0)))), 1.0);
   
#if defined(_P1_DEBUG_)

    #define DEBUG_LINE(a,v,p) if((v_av2TexCoord[0].a > (v-0.001)) && (v_av2TexCoord[0].a < (v+0.001))) gl_FragColor.rgb = mix(vec3(0.0,1.0,0.0), gl_FragColor.rgb, p);

         DEBUG_LINE(x, 0.25,  0.6) else DEBUG_LINE(y, 0.25,  0.6)
    else DEBUG_LINE(x, 0.50,  0.6) else DEBUG_LINE(y, 0.50,  0.6)
    else DEBUG_LINE(x, 0.75,  0.6) else DEBUG_LINE(y, 0.75,  0.6)
    else DEBUG_LINE(x, 0.125, 0.8) else DEBUG_LINE(y, 0.125, 0.8)
    else DEBUG_LINE(x, 0.375, 0.8) else DEBUG_LINE(y, 0.375, 0.8)
    else DEBUG_LINE(x, 0.625, 0.8) else DEBUG_LINE(y, 0.625, 0.8)
    else DEBUG_LINE(x, 0.875, 0.8) else DEBUG_LINE(y, 0.875, 0.8)

#endif
}