///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


#if defined(_CORE_VERTEX_SHADER_)

    vec2 AspectSize()
    {
        // 
        return abs(max(u_v4Resolution.xy * u_v4Resolution.wz, 1.0) * (u_v2TwoSize * u_v4Resolution.zw * 2.0));
    }

#elif defined(_CORE_FRAGMENT_SHADER_)

    // constant values
    const float c_v1White = 0.8;    // white contrast value (to reduce eye strain)
    const float c_v1Black = 0.04;   // black contrast value

    vec2 DistortionStrength(const in vec2 v2TexCoord)
    {
        // 
        return 1.0 - coreSmoothStep(0.85, 0.99, abs(v2TexCoord * 2.0 - 1.0));
    }

#endif