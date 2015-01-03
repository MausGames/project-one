//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIP_H_
#define _P1_GUARD_SHIP_H_


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
private:
    coreModelPtr m_pModelLow;   // low-polygon model object (used for depth-only, shadow, outline-effect)


public:
    cShip()noexcept;
    virtual ~cShip();

    // define the visual appearance
    inline const coreModelPtr& DefineModelLow(const coreModelPtr& pModel) {m_pModelLow = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow(const char*         pcName) {m_pModelLow = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship
    void Render(const coreProgramPtr& pProgram)override;


private:
    DISABLE_COPY(cShip)
};


#endif // _P1_GUARD_SHIP_H_