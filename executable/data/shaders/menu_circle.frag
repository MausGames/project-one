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
    //if(dot(v_av2TexCoord[0], u_v2TexOffset) > 0.0 &&
    //   dot(v_av2TexCoord[0].yx, vec2(1.0,0.0)) > 0.0) discard;

    //float v1Detail = coreTexture2D(0, v_av2TexCoord[1]).r * 16.0 - 1.0;
    //v1Detail = (v1Detail + 1.0) * 0.25;
    
    //if(v1Value > 1.0) discard;
    //if(v1Value < 0.6) discard;
    
    //vec3 v3Texture = vec3(1.0) - coreTexture2D(0, v_av2TexCoord[1]).rgb;
    
    float v1Value = coreLengthSq(v_av2TexCoord[0]) * 1.07;
    
    vec3  v3Color;
    float v1Alpha;
    
    if(u_v2TexOffset.x < 0.5*PI + (atan(-v_av2TexCoord[0].y / v_av2TexCoord[0].x) + ((v_av2TexCoord[0].x < 0.0) ? PI*0.0 : PI*1.0))) 
    {
        v3Color = u_v4Color.rgb * 0.2;
        v1Alpha = smoothstep(0.62, 0.7, v1Value) * (1.0 - smoothstep(0.88, 0.98, v1Value));
    }
    else
    {
        v3Color = ((smoothstep(0.6,  0.68, v1Value) * (1.0 - smoothstep(0.9, 1.0, v1Value))) * 0.8 + 0.2) * u_v4Color.rgb;
        v1Alpha =   smoothstep(0.52, 0.6,  v1Value) * (1.0 - smoothstep(1.0, 1.1, v1Value));
    }
    
    //float v1Alpha = smoothstep(0.57, 0.63, v1Value) * (1.0 - smoothstep(0.96, 1.04, v1Value));
    //float v1Alpha = 1.0;
    
    // draw texture with subtle white glow
    gl_FragColor   = vec4(v3Color * v1Alpha, v1Alpha * u_v4Color.a);
}