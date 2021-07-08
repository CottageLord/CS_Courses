// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "EntityStats.h"
#include "iPlayer.h"
#include "Vec2.h"

class Entity 
{

public:
    Entity(const iEntityStats& stats, const Vec2& pos, bool isNorth);
    virtual ~Entity() {}

    virtual const iEntityStats& getStats() const { return m_Stats; }

    virtual void tick(float deltaTSec);

    virtual bool isNorth() const { return m_bNorth; }
    virtual bool isHittingWall() const { return m_bHitWall; }
    virtual bool isInRiver() const { return m_bInRiver; }

    virtual bool isDead() const { return m_Health <= 0; }
    virtual int getHealth() const { return m_Health; }
    void takeDamage(int dmg) { m_Health -= dmg; }

    virtual const Vec2& getPosition() const { return m_Pos; }
    virtual const Vec2& getWallPosition() const { return m_WallPos; }
    virtual const Vec2& getMoveVec() const { return m_MoveVec; }
    void bePushedAway(float horizontalShift, float verticalShift);
    iPlayer::EntityData getData() const { return iPlayer::EntityData(m_Stats, m_Health, m_Pos); }

protected:
    void pickTarget();
    bool targetInRange();

protected:
    const iEntityStats& m_Stats;
    bool m_bNorth;
    int m_Health;
    bool m_bHitWall;
    bool m_bInRiver;
    Vec2 m_WallPos;
    Vec2 m_Pos;
    Vec2 m_MoveVec;

    // Our target will be the closest target (may change every tick) until
    //  we attack it.  Once we attack a target, we stay locked on it until
    //  it dies
    Entity* m_pTarget;
    bool m_bTargetLock;
    float m_TimeSinceAttack;
};
