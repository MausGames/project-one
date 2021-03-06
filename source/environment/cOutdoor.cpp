///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cOutdoor::cOutdoor()noexcept
: m_aiHeight      {}
, m_iVertexOffset (0u)
, m_iIndexOffset  (0u)
, m_fFlyOffset    (0.0f)
, m_iHandleIndex  (0u)
, m_iAlgorithm    (0u)
, m_fGrade        (0.0f)
, m_afLerpMul     {}
, m_afLerpAdd     {}
, m_aiLerpRange   {}
, m_afLerpData    {}
{
}

cOutdoor::cOutdoor(const coreChar* pcTextureTop, const coreChar* pcTextureBottom, const coreUint8 iAlgorithm, const coreFloat fGrade)noexcept
: cOutdoor ()
{
    const coreTextureSpec oSpec = CORE_GL_SUPPORT(ARB_texture_rg) ? CORE_TEXTURE_SPEC_R8 : CORE_TEXTURE_SPEC_RGB8;

    // set object properties
    this->SetPosition(coreVector3(0.0f,0.0f,0.0f));
    this->LerpHeightNow(1.0f, 0.0f);

    // 
    m_LightMap.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oSpec);
    m_LightMap.Create(g_vGameResolution * OUTDOOR_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // load outdoor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    this->LoadGeometry(iAlgorithm, fGrade);

    // load outdoor textures
    m_pNormalMap = Core::Manager::Resource->LoadNew<coreTexture>();
    this->LoadTextures(pcTextureTop, pcTextureBottom);

    // load outdoor shader-program
    this->LoadProgram(false);
}


// ****************************************************************
// destructor
cOutdoor::~cOutdoor()
{
    // free resources
    this->DefineTexture(2u, NULL);
    Core::Manager::Resource->Free(&m_pModel);
    Core::Manager::Resource->Free(&m_pNormalMap);
}


// ****************************************************************
// render the outdoor-surface
void cOutdoor::Render()
{
    // 
    this->__Render(m_pDefaultProgram, [this]()
    {
        // 
        cShadow::EnableShadowRead(m_iHandleIndex);

        // enable all active textures
        coreTexture::EnableAll(m_apTexture);
    });

    // enable light map
    if(m_LightMap.GetIdentifier()) m_LightMap.GetColorTarget(0u).pTexture->Enable(3u);
}

void cOutdoor::RenderLight()
{
    // 
    this->__Render(m_pLightProgram, [this]()
    {
        // 
        cShadow::EnableShadowRead(m_iHandleIndex + SHADOW_HANDLE_OUTDOOR_LIGHT);
    });
}

void cOutdoor::RenderDepth()
{
    // 
    this->__Render(m_pLightProgram, [](){});
}


// ****************************************************************
// load outdoor geometry
void cOutdoor::LoadGeometry(const coreUint8 iAlgorithm, const coreFloat fGrade)
{
    BIG_STATIC sVertex     aVertexData[OUTDOOR_TOTAL_VERTICES];
    BIG_STATIC coreUint16  aiIndexData[OUTDOOR_TOTAL_INDICES];
    BIG_STATIC coreVector3 avOrtho1   [OUTDOOR_TOTAL_VERTICES]; std::memset(avOrtho1, 0, sizeof(avOrtho1));
    BIG_STATIC coreVector3 avOrtho2   [OUTDOOR_TOTAL_VERTICES]; std::memset(avOrtho2, 0, sizeof(avOrtho2));

    // delete old data
    m_pModel->Unload();
    std::memset(m_aiHeight, 0, sizeof(m_aiHeight));

    // save properties
    m_iAlgorithm = iAlgorithm;
    m_fGrade     = fGrade;

    // select algorithm function
    coreFloat (*nAlgorithmFunc) (const coreFloat, const coreFloat);
    switch(iAlgorithm)
    {
    default: ASSERT(false)
    case 1u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r = -(COS((x - I_TO_F(OUTDOOR_WIDTH / 2u)) * 0.087f*PI) * 10.0f);                                                                                              return r;}; break;
    case 2u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) + SIN(x*0.075f*PI)) * 8.0f - 6.0f);                                                                                                 return r;}; break;
    case 3u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) * 0.25f - ((x+0.5f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 4.0f) * 20.0f - 13.0f);                                                       return r;}; break;
    case 4u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r = -(ABS(SIN(y*0.150f*PI) * 0.25f - ((x+0.0f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 2.0f) * 20.0f - 10.0f) * SIN(y*0.150f *PI) - 0.0f; if(r <    0.0f) r = -1.0f; return r;}; break;
    case 5u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) * 0.25f + ((x+0.0f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 2.0f) * 20.0f - 10.0f) * SIN(y*0.075f *PI) - 1.5f; if(r < -100.0f) r = -1.0f; return r;}; break;
    case 6u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) * 0.25f - ((x+0.0f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 4.0f) * 20.0f - 10.0f) * SIN(y*0.150f *PI) - 1.0f;                            return r;}; break;
    case 7u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) * 0.25f - ((x+0.0f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 4.0f) * 20.0f - 10.0f) * SIN(y*0.075f *PI) + 3.0f;                            return r;}; break;
    case 8u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r =  (ABS(SIN(y*0.075f*PI) * 0.25f - ((x+0.0f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 4.0f) * 20.0f - 10.0f) * SIN(y*0.0375f*PI) - 1.0f;                            return r;}; break;
    case 9u: nAlgorithmFunc = [](const coreFloat x, const coreFloat y) {coreFloat r = -(COS((x - I_TO_F(OUTDOOR_WIDTH / 2u)) * 0.087f*PI) * 10.0f + SIN(y*0.087f*PI) * 10.0f) + 7.0f;                                                            return r;}; break;
    }

    // create vertices
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const coreInt32 x = i % OUTDOOR_WIDTH;
        const coreInt32 y = i / OUTDOOR_WIDTH;

        // calculate level (height) of the vertex
        coreFloat fLevel = nAlgorithmFunc(I_TO_F(x), I_TO_F(y));
        if(fGrade)
        {
            // add randomness to the level and smooth out water-intersection-area
            coreFloat fSmoothLevel;
            do {fSmoothLevel = CLAMP(fLevel, -fGrade*1.5f, fGrade*1.5f) + Core::Rand->Float(-0.5f, 0.5f)*fGrade;}
            while(coreMath::IsNear(fSmoothLevel, 0.0f, fGrade*0.25f));

            // forward smooth level
            fLevel = fSmoothLevel;
        }

        // center level at water
        fLevel += WATER_HEIGHT;

        // save height value
        m_aiHeight[i] = coreMath::Float32To16(fLevel);

        // set vertex position
        aVertexData[i].vPosition = coreVector3(I_TO_F(x - OUTDOOR_WIDTH / 2u) * OUTDOOR_DETAIL, I_TO_F(y - OUTDOOR_VIEW / 2u) * OUTDOOR_DETAIL, fLevel);
    }

    // sync beginning and ending height to create an infinite looking grid when resetting the position
    for(coreUintW i = 0u; i < OUTDOOR_VIEW; ++i)
    {
        for(coreUintW j = 0u; j < OUTDOOR_WIDTH; ++j)
        {
            const coreUintW f = i * OUTDOOR_WIDTH + j;
            const coreUintW t = f + OUTDOOR_HEIGHT * OUTDOOR_WIDTH;

            aVertexData[t].vPosition.z = aVertexData[f].vPosition.z;
            m_aiHeight [t]             = m_aiHeight [f];
        }
    }

    // create indices
    coreUintW iIndex = 0u;
    for(coreUintW y = 0u; y < OUTDOOR_BLOCKS_Y; ++y)
    {
        for(coreUintW x = 0u; x < OUTDOOR_BLOCKS_X; ++x)
        {
            const coreUint16 iVertex = x + y*OUTDOOR_WIDTH;

            aiIndexData[iIndex+0u] = iVertex;
            aiIndexData[iIndex+1u] = iVertex + 1u + OUTDOOR_WIDTH;
            aiIndexData[iIndex+2u] = iVertex      + OUTDOOR_WIDTH;
            aiIndexData[iIndex+3u] = iVertex;
            aiIndexData[iIndex+4u] = iVertex + 1u;
            aiIndexData[iIndex+5u] = iVertex + 1u + OUTDOOR_WIDTH;

            iIndex += 6u;
        }
    }

    // calculate normals
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const coreInt32 x = i % OUTDOOR_WIDTH;
        const coreInt32 y = i / OUTDOOR_WIDTH;

        const coreUintW iLeft  = MAX(x-1, 0)                                + y*OUTDOOR_WIDTH;
        const coreUintW iRight = MIN(x+1, coreInt32(OUTDOOR_WIDTH)      -1) + y*OUTDOOR_WIDTH;
        const coreUintW iDown  = MAX(y-1, 0)                               *OUTDOOR_WIDTH + x;
        const coreUintW iUp    = MIN(y+1, coreInt32(OUTDOOR_HEIGHT_FULL)-1)*OUTDOOR_WIDTH + x;

        aVertexData[i].vNormal = coreVector3::Cross((aVertexData[iLeft].vPosition - aVertexData[iRight].vPosition).NormalizedUnsafe(),
                                                    (aVertexData[iDown].vPosition - aVertexData[iUp   ].vPosition).NormalizedUnsafe());
    }

    // calculate tangents
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_INDICES; i += 3u)
    {
        // calculate triangle sides
        const coreVector3 A1 = aVertexData[aiIndexData[i+1u]].vPosition - aVertexData[aiIndexData[i+0u]].vPosition;
        const coreVector3 A2 = aVertexData[aiIndexData[i+2u]].vPosition - aVertexData[aiIndexData[i+0u]].vPosition;
        const coreVector2 B1 = coreVector2(0.25f, (i%2u) ? 0.0f : 0.25f);
        const coreVector2 B2 = coreVector2((i%2u) ? 0.25f : 0.0f, 0.25f);

        // calculate local tangent vector parameters
        const coreFloat   R  = RCP(B1.x*B2.y - B2.x*B1.y);
        const coreVector3 D1 = (A1*B2.y - A2*B1.y) * R;
        const coreVector3 D2 = (A2*B1.x - A1*B2.x) * R;

        for(coreUintW j = 0u; j < 3u; ++j)
        {
            // add local values to each point of the triangle
            avOrtho1[aiIndexData[i+j]] += D1;
            avOrtho2[aiIndexData[i+j]] += D2;
        }
    }
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        // finish the Gram-Schmidt process to calculate the tangent vector and bitangent sign (w)
        aVertexData[i].vTangent = coreVector4((avOrtho1[i] - aVertexData[i].vNormal * coreVector3::Dot(aVertexData[i].vNormal, avOrtho1[i])).NormalizedUnsafe(),
                                              SIGN(coreVector3::Dot(coreVector3::Cross(aVertexData[i].vNormal, avOrtho1[i]), avOrtho2[i])));
    }

    GLuint iNormFormat;
    coreUint32 (*nPackFunc) (const coreVector4 vVector);
    if(CORE_GL_SUPPORT(ARB_vertex_type_2_10_10_10_rev))
    {
        // use high-quality packed format
        iNormFormat = GL_INT_2_10_10_10_REV;
        nPackFunc   = [](const coreVector4 vVector) {return vVector.PackSnorm210();};
    }
    else
    {
        // use low-quality byte format
        iNormFormat = GL_BYTE;
        nPackFunc   = [](const coreVector4 vVector) {return vVector.PackSnorm4x8();};
    }

    // reduce total vertex size
    BIG_STATIC sVertexPacked aPackedData[OUTDOOR_TOTAL_VERTICES];
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const sVertex& oVertex = aVertexData[i];

        // convert vertex attributes
        aPackedData[i].fHeight  = oVertex.vPosition.z;
        aPackedData[i].iNormal  = nPackFunc(coreVector4(oVertex.vNormal, 0.0f));
        aPackedData[i].iTangent = nPackFunc(oVertex.vTangent);
    }

    // create vertex buffer
    coreVertexBuffer* pBuffer = m_pModel->CreateVertexBuffer(OUTDOOR_TOTAL_VERTICES, sizeof(sVertexPacked), aPackedData, CORE_DATABUFFER_STORAGE_STATIC);
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_POSITION_NUM, 1u, GL_FLOAT,    false, 0u);
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_NORMAL_NUM,   4u, iNormFormat, false, 1u*sizeof(coreFloat));
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_TANGENT_NUM,  4u, iNormFormat, false, 1u*sizeof(coreFloat) + 1u*sizeof(coreUint32));

    // create index buffer
    m_pModel->CreateIndexBuffer(OUTDOOR_TOTAL_INDICES, sizeof(coreUint16), aiIndexData, CORE_DATABUFFER_STORAGE_STATIC);

    if(!CORE_GL_SUPPORT(EXT_gpu_shader4))
    {
        // 
        BIG_STATIC coreVector2 avPosition[OUTDOOR_TOTAL_VERTICES];
        for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i) avPosition[i] = aVertexData[i].vPosition.xy();

        // 
        pBuffer = m_pModel->CreateVertexBuffer(OUTDOOR_TOTAL_VERTICES, sizeof(coreVector2), avPosition, CORE_DATABUFFER_STORAGE_STATIC);
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_TEXCOORD_NUM, 2u, GL_FLOAT, false, 0u);
    }

    Core::Log->Info("Outdoor-Geometry (%u, %.1f) loaded", iAlgorithm, fGrade);
}


// ****************************************************************
// load outdoor textures
void cOutdoor::LoadTextures(const coreChar* pcTextureTop, const coreChar* pcTextureBottom)
{
    // load color textures
    this->DefineTexture(0u, PRINT("environment_%s_diff.png", pcTextureTop));
    this->DefineTexture(1u, PRINT("environment_%s_diff.png", pcTextureBottom));

    // unbind normal map to prevent concurrency problems
    this->DefineTexture(2u, NULL);

    // delete sync object
    m_Sync.Delete();

    Core::Manager::Resource->AttachFunction([=, this]()
    {
        // check for sync object status
        const coreStatus eCheck = m_Sync.Check(0u, CORE_SYNC_CHECK_FLUSHED);
        if(eCheck == CORE_OK) this->DefineTexture(2u, m_pNormalMap);
        if(eCheck >= CORE_OK) return eCheck;

        // delete old data
        m_pNormalMap->Unload();

        // retrieve normal map files
        coreFileScope pFile1 = Core::Manager::Resource->RetrieveFile(PRINT("data/textures/environment_%s_norm.png", pcTextureTop));
        coreFileScope pFile2 = Core::Manager::Resource->RetrieveFile(PRINT("data/textures/environment_%s_norm.png", pcTextureBottom));

        // decompress files to plain pixel data
        coreSurfaceScope pSurface1 = IMG_LoadTyped_RW(pFile1->CreateReadStream(), true, coreData::StrExtension(pFile1->GetPath()));
        coreSurfaceScope pSurface2 = IMG_LoadTyped_RW(pFile2->CreateReadStream(), true, coreData::StrExtension(pFile2->GetPath()));
        WARN_IF(!pSurface1 || (pSurface1->format->BitsPerPixel != 24u)) return CORE_OK;
        WARN_IF(!pSurface2 || (pSurface2->format->BitsPerPixel != 24u)) return CORE_OK;

        // allocate required memory
        const coreUintW iSize = pSurface1->w * pSurface1->h * 4u;
        coreByte* pOutput = new coreByte[iSize];
        coreByte* pInput1 = s_cast<coreByte*>(pSurface1->pixels);
        coreByte* pInput2 = s_cast<coreByte*>(pSurface2->pixels);

        // merge XY components of both normal maps (divided by Z, partial-derivative)
        for(coreUintW i = 0u, j = 0u; i < iSize; i += 4u, j += 3u)
        {
            const coreFloat x1 =             (coreFloat(*(pInput1 + j))      - 127.5f);
            const coreFloat y1 =             (coreFloat(*(pInput1 + j + 1u)) - 127.5f);
            const coreFloat z1 = 127.5f * RCP(coreFloat(*(pInput1 + j + 2u)) - 127.5f);
            const coreFloat x2 =             (coreFloat(*(pInput2 + j))      - 127.5f);
            const coreFloat y2 =             (coreFloat(*(pInput2 + j + 1u)) - 127.5f);
            const coreFloat z2 = 127.5f * RCP(coreFloat(*(pInput2 + j + 2u)) - 127.5f);

            const coreFloat xz1 = x1 * z1 + 127.5f;
            const coreFloat yz1 = y1 * z1 + 127.5f;
            const coreFloat xz2 = x2 * z2 + 127.5f;
            const coreFloat yz2 = y2 * z2 + 127.5f;

            ASSERT((xz1 <= 255.0f) && (yz1 <= 255.0f) &&
                   (xz2 <= 255.0f) && (yz2 <= 255.0f))

            const coreUint8 aiPixel[] = {coreUint8(xz1), coreUint8(yz1),
                                         coreUint8(xz2), coreUint8(yz2)};

            std::memcpy(pOutput + i, aiPixel, sizeof(aiPixel));
        }

        // create final normal map
        m_pNormalMap->Create(pSurface1->w, pSurface1->h, CORE_TEXTURE_SPEC_RGBA8, CORE_TEXTURE_MODE_FILTER | CORE_TEXTURE_MODE_REPEAT);
        m_pNormalMap->Modify(0u, 0u, pSurface1->w, pSurface1->h, iSize, pOutput);

        // free required memory
        SAFE_DELETE_ARRAY(pOutput)

        Core::Log->Info("Outdoor-Textures (%s, %s) loaded", pcTextureTop, pcTextureBottom);
        return m_Sync.Create() ? CORE_BUSY : CORE_OK;
    });
}


// ****************************************************************
// load outdoor shader-program
void cOutdoor::LoadProgram(const coreBool bGlow)
{
    // 
    m_iHandleIndex = bGlow ? SHADOW_HANDLE_OUTDOOR_GLOW : SHADOW_HANDLE_OUTDOOR;

    // 
    m_pDefaultProgram = Core::Manager::Resource->Get<coreProgram>(bGlow ? "environment_outdoor_glow_program"       : "environment_outdoor_program");
    m_pLightProgram   = Core::Manager::Resource->Get<coreProgram>(bGlow ? "environment_outdoor_light_glow_program" : "environment_outdoor_light_program");
}


// ****************************************************************
// retrieve height value
FUNC_PURE coreFloat cOutdoor::RetrieveHeight(const coreVector2 vPosition)const
{
    // 
    const coreMatrix2 mRota = coreMatrix3::Rotation(this->GetDirection().xy()).m12();
    return this->RetrieveBackHeight(vPosition * mRota - this->GetPosition().xy());
}

FUNC_PURE coreFloat cOutdoor::RetrieveBackHeight(const coreVector2 vPosition)const
{
    // convert real position to block position
    const coreFloat fX = vPosition.x / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_WIDTH / 2u);
    const coreFloat fY = vPosition.y / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_VIEW  / 2u);
    ASSERT((F_TO_UI(fX) < OUTDOOR_WIDTH) && (F_TO_UI(fY) < OUTDOOR_HEIGHT_FULL))

    // retrieve all four corners of the block
    const coreUintW iI00 = F_TO_UI(fY) * OUTDOOR_WIDTH + F_TO_UI(fX);   // bottom left
    const coreUintW iI01 = iI00 + 1u;                                   // bottom right
    const coreUintW iI10 = iI00      + OUTDOOR_WIDTH;                   // top left
    const coreUintW iI11 = iI00 + 1u + OUTDOOR_WIDTH;                   // top right
    ASSERT((iI00 < OUTDOOR_TOTAL_VERTICES) && (iI11 < OUTDOOR_TOTAL_VERTICES))

    // retrieve height values of the corners
    const coreFloat fH00 = coreMath::Float16To32(m_aiHeight[iI00]);
    const coreFloat fH01 = coreMath::Float16To32(m_aiHeight[iI01]);
    const coreFloat fH10 = coreMath::Float16To32(m_aiHeight[iI10]);
    const coreFloat fH11 = coreMath::Float16To32(m_aiHeight[iI11]);

    // interpolate between all height values
    const coreFloat fFractX = FRACT(fX);
    const coreFloat fFractY = FRACT(fY);
    const coreFloat fHeight = LERP(LERP(fH00, fH01, fFractX), LERP(fH10, fH11, fFractX), fFractY);

    // 
    const coreVector2 vLerp = this->CalcLerpVector(vPosition.y);
    return fHeight * vLerp.x + vLerp.y;
}


// ****************************************************************
// 
FUNC_PURE coreVector3 cOutdoor::RetrieveNormal(const coreVector2 vPosition)const
{
    // 
    const coreMatrix2 mRota = coreMatrix3::Rotation(this->GetDirection().xy()).m12();
    return this->RetrieveBackNormal(vPosition * mRota - this->GetPosition().xy());
}

FUNC_PURE coreVector3 cOutdoor::RetrieveBackNormal(const coreVector2 vPosition)const
{
    constexpr coreFloat fWidth = OUTDOOR_DETAIL * 0.35f;

    // 
    const coreFloat A = this->RetrieveBackHeight(vPosition + coreVector2(0.0f, fWidth));
    const coreFloat B = this->RetrieveBackHeight(vPosition + coreVector2(fWidth, 0.0f));
    const coreFloat C = this->RetrieveBackHeight(vPosition - coreVector2(0.0f, fWidth));
    const coreFloat D = this->RetrieveBackHeight(vPosition - coreVector2(fWidth, 0.0f));

    // 
    return coreVector3::Cross(coreVector3(fWidth * -2.0f, 0.0f, D - B).NormalizedUnsafe(),
                              coreVector3(0.0f, fWidth * -2.0f, C - A).NormalizedUnsafe()).NormalizedUnsafe();
}


// ****************************************************************
// retrieve ray intersection point
FUNC_PURE coreVector3 cOutdoor::RetrieveIntersect(const coreVector3 vRayPosition, const coreVector3 vRayDirection)const
{
    ASSERT(vRayDirection.z < 0.0f)
    coreVector3 vOutput = vRayPosition;

    // naive ray-tracing with fixed step-size
    for(coreUintW i = 100u; i--; )
    {
        ASSERT(i)

        // check for boundaries
        if(ABS(vOutput.x) > I_TO_F(OUTDOOR_WIDTH / 2u) * OUTDOOR_DETAIL ||
           ABS(vOutput.y) > I_TO_F(OUTDOOR_VIEW  / 2u) * OUTDOOR_DETAIL)
           break;

        // retrieve and check height at current position
        const coreFloat fCurHeight = this->RetrieveHeight(vOutput.xy());
        if(vOutput.z < fCurHeight)
        {
            // intersection found, step back to improve precision
            vOutput += vRayDirection * ((fCurHeight - vOutput.z) * RCP(vRayDirection.z));   // # Z is negative
            break;
        }

        // next step
        vOutput += vRayDirection * 2.0f;
    }

    return vOutput;
}


// ****************************************************************
// 
void cOutdoor::LerpHeight(const coreFloat fMul, const coreFloat fAdd, const coreUint16 iRange)
{
    ASSERT(!this->IsLerping())

    // 
    m_afLerpMul  [0] = m_afLerpMul[1];
    m_afLerpAdd  [0] = m_afLerpAdd[1];
    m_aiLerpRange[0] = 0u;

    // 
    m_afLerpMul  [1] = fMul;
    m_afLerpAdd  [1] = fAdd;
    m_aiLerpRange[1] = iRange + 2u * OUTDOOR_VIEW;
}


// ****************************************************************
// 
void cOutdoor::LerpHeightNow(const coreFloat fMul, const coreFloat fAdd)
{
    ASSERT(!this->IsLerping())

    // 
    m_afLerpMul  [0] = m_afLerpMul  [1] = fMul;
    m_afLerpAdd  [0] = m_afLerpAdd  [1] = fAdd;
    m_aiLerpRange[0] = m_aiLerpRange[1] = 0u;

    // 
    m_afLerpData[0] = m_afLerpData[2] = m_afLerpData[4] = fMul;
    m_afLerpData[1] = m_afLerpData[3] = m_afLerpData[5] = fAdd;
    m_afLerpData[6] = CORE_MATH_PRECISION;
}


// ****************************************************************
// 
FUNC_LOCAL coreVector2 cOutdoor::CalcLerpVector(const coreFloat fPositionY)const
{
    if(this->IsLerping())
    {
        // 
        const coreFloat fLerp = (fPositionY + this->GetPosition().y) / (OUTDOOR_DETAIL * I_TO_F(OUTDOOR_VIEW)) + 0.5f;
        if(fLerp <= m_afLerpData[6])
        {
            return LERP(r_cast<const coreVector2&>(m_afLerpData[0]), r_cast<const coreVector2&>(m_afLerpData[4]), fLerp * RCP(m_afLerpData[6]));
        }
        else
        {
            return LERP(r_cast<const coreVector2&>(m_afLerpData[4]), r_cast<const coreVector2&>(m_afLerpData[2]), (fLerp - m_afLerpData[6]) * RCP(1.0f - m_afLerpData[6]));
        }
    }
    else
    {
        // 
        return r_cast<const coreVector2&>(m_afLerpData[0]);
    }
}


// ****************************************************************
// 
void cOutdoor::UpdateLightMap()
{
    if(!m_LightMap.GetIdentifier()) return;

    // 
    m_LightMap.StartDraw();
    this->RenderLight();
}


// ****************************************************************
// 
void cOutdoor::InvalidateLightMap()
{
    if(!m_LightMap.GetIdentifier()) return;

    // 
    m_LightMap.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// set current fly offset
void cOutdoor::SetFlyOffset(const coreFloat fFlyOffset)
{
    if(this->IsLerping() && (F_TO_UI(m_fFlyOffset) != F_TO_UI(fFlyOffset)))
    {
        coreUint16& A = m_aiLerpRange[0];   // current
        coreUint16& B = m_aiLerpRange[1];   // target

        // 
        ++A;

        // 
        const coreBool bBegin = ((A)     < OUTDOOR_VIEW);
        const coreBool bEnd   = ((B - A) < OUTDOOR_VIEW);

        // 
        const coreFloat fDist = RCP(I_TO_F(B - OUTDOOR_VIEW));
        const coreFloat fFrom = bBegin ? 0.0f : (I_TO_F(A - OUTDOOR_VIEW) * fDist);
        const coreFloat fTo   = bEnd   ? 1.0f : (I_TO_F(A)                * fDist);

        // 
        m_afLerpData[0] = LERP(m_afLerpMul[0], m_afLerpMul[1], fFrom);
        m_afLerpData[1] = LERP(m_afLerpAdd[0], m_afLerpAdd[1], fFrom);
        m_afLerpData[2] = LERP(m_afLerpMul[0], m_afLerpMul[1], fTo);
        m_afLerpData[3] = LERP(m_afLerpAdd[0], m_afLerpAdd[1], fTo);
        m_afLerpData[4] = bBegin ? m_afLerpData[0] : m_afLerpData[2];
        m_afLerpData[5] = bBegin ? m_afLerpData[1] : m_afLerpData[3];
        m_afLerpData[6] = bBegin ? (1.0f - (I_TO_F(A) / I_TO_F(OUTDOOR_VIEW))) : (bEnd ? (I_TO_F(B - A) / I_TO_F(OUTDOOR_VIEW)) : 1.0f);

        // 
        m_afLerpData[6] = CLAMP(m_afLerpData[6], CORE_MATH_PRECISION, 1.0f - CORE_MATH_PRECISION);

        // 
        if((m_fFlyOffset > fFlyOffset) && ((m_fFlyOffset - fFlyOffset) < I_TO_F(OUTDOOR_VIEW)))
        {
            std::swap(m_afLerpData[0], m_afLerpData[2]);
            std::swap(m_afLerpData[1], m_afLerpData[3]);
            m_afLerpData[6] = 1.0f - m_afLerpData[6];
        }
    }

    // set new value
    m_fFlyOffset = fFlyOffset;
    ASSERT(F_TO_UI(m_fFlyOffset) < OUTDOOR_HEIGHT)

    // calculate vertex and index offset
    m_iVertexOffset = F_TO_UI(m_fFlyOffset) * OUTDOOR_WIDTH;
    m_iIndexOffset  = F_TO_UI(m_fFlyOffset) * OUTDOOR_BLOCKS_X * OUTDOOR_PER_INDICES * sizeof(coreUint16);
}


// ****************************************************************
// 
void cOutdoor::SetTransform(const coreFloat fFlyOffset, const coreFloat fSideOffset, const coreVector2 vDirection)
{
    // (only used for height-calculations, not for shading) 
    this->SetPosition (coreVector3(-fSideOffset, fFlyOffset * -OUTDOOR_DETAIL, 0.0f));
    this->SetDirection(coreVector3( vDirection,                                0.0f));
}