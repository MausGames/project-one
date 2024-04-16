//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cHeadlight::cHeadlight()noexcept
{
    const coreTextureSpec oSpec = CORE_GL_SUPPORT(ARB_texture_rg) ? CORE_TEXTURE_SPEC_R8 : CORE_TEXTURE_SPEC_RGB8;

    // 
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oSpec);
    m_FrameBuffer.Create(g_vGameResolution * HEADLIGHT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // load object resources
    this->DefineProgram("effect_headlight_program");
    this->DefineTexture(0u, m_FrameBuffer.GetColorTarget(0u).pTexture);

    // set object properties
    this->SetSize(coreVector2(1.0f,1.0f));
    this->Move();

    // 
    m_Spot .DefineProgram("default_2d_program");
    m_Spot .DefineTexture(0u, "effect_headlight_spot.png");
    m_Point.DefineProgram("default_2d_program");
    m_Point.DefineTexture(0u, "effect_headlight_point.png");
}


// ****************************************************************
// destructor
cHeadlight::~cHeadlight()
{
    // explicitly undefine to detach textures
    this->Undefine();
}


// ****************************************************************
// 
void cHeadlight::Render()
{
    // 
    this->coreObject2D::Render();

    // 
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// 
void cHeadlight::Update()
{
    // 
    if(!CORE_GL_SUPPORT(ARB_texture_rg)) glColorMask(true, false, false, false);

    if(!m_aSpotCommand.empty() || !m_aPointCommand.empty())
    {
        // 
        m_FrameBuffer.StartDraw();
        m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
        {
            // 
            glBlendEquation(GL_MAX);
            {
                // 
                FOR_EACH(it, m_aSpotCommand)
                {
                    m_Spot.SetPosition (it->vPosition);
                    m_Spot.SetSize     (it->vSize);
                    m_Spot.SetDirection(it->vDirection);

                    m_Spot.Move();
                    m_Spot.Render();
                }

                // 
                FOR_EACH(it, m_aPointCommand)
                {
                    m_Point.SetPosition(it->vPosition);
                    m_Point.SetSize    (it->vSize);

                    m_Point.Move();
                    m_Point.Render();
                }
            }
            glBlendEquation(GL_FUNC_ADD);
        }

        // 
        m_aSpotCommand .clear();
        m_aPointCommand.clear();
    }
    else m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);

    // 
    if(!CORE_GL_SUPPORT(ARB_texture_rg)) glColorMask(true, true, true, true);
}


// ****************************************************************
// 
void cHeadlight::UpdateDefault()
{
    if(g_pGame)
    {
        // 
        g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
        {
            this->DrawSpot(pPlayer->GetPosition() + 49.0f * pPlayer->GetDirection(), coreVector2(100.0f,100.0f), pPlayer->GetDirection().xy());
        });

        // 
        g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
        {
            this->DrawPoint(pPlayer);
        });
        //g_pGame->GetEnemyManager()->ForEachEnemy([this](const cEnemy* pEnemy)
        //{
        //    this->DrawPoint(pEnemy);
        //});
    }

    // 
    this->Update();
}


// ****************************************************************
// 
void cHeadlight::DrawSpot(const coreVector3& vPosition, const coreVector2& vSize, const coreVector2& vDirection)
{
    // 
    sSpotCommand oCommand;
    oCommand.vPosition  = g_pForeground->Project2D(vPosition)                * HEADLIGHT_SCALE_FACTOR;
    oCommand.vSize      = g_pForeground->Project2D(coreVector3(vSize, 0.0f)) * HEADLIGHT_SCALE_FACTOR;
    oCommand.vDirection = vDirection.InvertedX();

    // 
    m_aSpotCommand.push_back(oCommand);
}


// ****************************************************************
// 
void cHeadlight::DrawPoint(const coreVector3& vPosition, const coreVector2& vSize)
{
    // 
    sPointCommand oCommand;
    oCommand.vPosition = g_pForeground->Project2D(vPosition)                * HEADLIGHT_SCALE_FACTOR;
    oCommand.vSize     = g_pForeground->Project2D(coreVector3(vSize, 0.0f)) * HEADLIGHT_SCALE_FACTOR;

    // 
    m_aPointCommand.push_back(oCommand);
}

void cHeadlight::DrawPoint(const coreObject3D* pObject)
{
    ASSERT(pObject)

    // 
    if(!pObject->GetModel().IsUsable()) return;
    this->DrawPoint(pObject->GetPosition(), coreVector2(3.0f,3.0f) * (pObject->GetModel()->GetBoundingRadius() * pObject->GetSize().Max()));
}


// ****************************************************************
// reset with the resource manager
void cHeadlight::__Reset(const coreResourceReset bInit)
{
    if(bInit) m_FrameBuffer.Create(g_vGameResolution * HEADLIGHT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_FrameBuffer.Delete();
}