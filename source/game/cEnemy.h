//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENEMY_H_
#define _P1_GUARD_ENEMY_H_


// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
private:


public:
    cEnemy()noexcept;
    virtual ~cEnemy();

    ENABLE_ID

    // render and move the enemy
    void Render()override;
    void Move  ()override;


private:
    // render and move routine for derived classes
    virtual void __RenderOwn() {}
    virtual void __MoveOwn()   {}
};


// ****************************************************************
// scout enemy class
class cScout final : public cEnemy
{
private:


public:
    cScout()noexcept;
    ~cScout();

    ASSIGN_ID(1, "Scout")
};


// ****************************************************************
// warrior enemy class
class cWarrior final : public cEnemy
{
private:


public:
    cWarrior()noexcept;
    ~cWarrior();

    ASSIGN_ID(2, "Warrior")
};


// ****************************************************************
// star enemy class
class cStar final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cStar()noexcept;
    ~cStar();

    ASSIGN_ID(3, "Star")


private:
    // move the star enemy
    void __MoveOwn()override;
};


// ****************************************************************
// arrow enemy class
class cArrow final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cArrow()noexcept;
    ~cArrow();

    ASSIGN_ID(4, "Arrow")


private:
    // move the arrow enemy
    void __MoveOwn()override;
};


// ****************************************************************
// miner enemy class
class cMiner final : public cEnemy
{
private:


public:
    cMiner()noexcept;
    ~cMiner();

    ASSIGN_ID(5, "Miner")
};


// ****************************************************************
// freezer enemy class
class cFreezer final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cFreezer()noexcept;
    ~cFreezer();

    ASSIGN_ID(6, "Freezer")


private:
    // move the freezer enemy
    void __MoveOwn()override;
};


// ****************************************************************
// cinder enemy class
class cCinder final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cCinder()noexcept;
    ~cCinder();

    ASSIGN_ID(7, "Cinder")


private:
    // move the cinder enemy
    void __MoveOwn()override;
};


#endif // _P1_GUARD_ENEMY_H_