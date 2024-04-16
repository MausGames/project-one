///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////

// TODO 3: anti-alias borders (would need 2 taps)


void FragmentMain()
{
    float v1Sin  = sin(v_av2TexCoord[1].x);
    float v1Cos  = cos(v_av2TexCoord[1].x);
    mat2  m2Rota = mat2(v1Cos, v1Sin, -v1Sin, v1Cos);

    vec2  v2Coord  = v_av2TexCoord[0];
    float v1Border = 0.0;
    float v1Thresh = 1.0;

    for(int i = 0; i < 5; ++i)
    {
        v2Coord = m2Rota * v2Coord;

        float v1Len = max(abs(v2Coord.x), abs(v2Coord.y));

        v1Thresh *= 0.65;

        if(v1Len >= v1Thresh)
        {
            v1Border = (1.0 - smoothstep(v1Thresh, v1Thresh + 0.1, v1Len)) * 0.2;
            break;
        }
    }

    float v1Value = coreTexture2D(0, v2Coord * 1.5).r;

    gl_FragColor = vec4(vec3(mix(v1Value, 1.0, v1Border)), u_v4Color.a);
}