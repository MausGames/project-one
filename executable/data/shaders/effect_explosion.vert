///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
varying float v_v1Light;   // 


void VertexMain()
{
/*
    // transform position normal-resized
    vec4 v4NewPosition = vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition + a_v3RawNormal * u_v2TexOffset.x) * u_v3Size) + u_v3Position, 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;

    // forward blink intensity
    v_v1Blink = u_v2TexOffset.y;

    // 
    vec3 v3ViewDir   = normalize(u_v3CamPosition - v4NewPosition.xyz);
    vec3 v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);

    // calculate light and color intensity
    float v1Base = dot(vec3(v3ViewDir.xy, abs(v3ViewDir.z)), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));
    v_v1Strength = 3.0 - 2.1 * v1Base;
    */
        
    // Normale berechnen
    vec3 n = a_v3RawNormal;//normalize(gl_NormalMatrix * gl_Normal);

    // Dehnungs-Animation berechnen
    //vec3 v3Lerp = mix(normalize(a_v3RawPosition), a_v3RawPosition, 10.0 * u_v4Color.a - 5.0);
    vec3 v3Lerp = mix((a_v3RawPosition), a_v3RawPosition, (1.0 - smoothstep(0.0, 1.0, u_v4Color.a)) * 5.0);

    // Vertexposition clippen
    vec4 v4NewPosition = vec4((v3Lerp * u_v3Size) + u_v3Position, 1.0);

    // Helligkeit berechnen
    v_v1Light = abs(n.z);//abs(dot(normalize(v3Lerp), n));
    v_v1Light = pow(v_v1Light, 10.0) * 5.0 + 2.6;

    // Vertexposition und Texturkoordinaten übernehmen
    gl_Position = u_m4ViewProj * v4NewPosition;
}