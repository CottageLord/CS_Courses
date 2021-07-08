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

#include "Mob.h"

#include "Constants.h"
#include "Game.h"
#include "Player.h"

#include <algorithm>
#include <vector>


Mob::Mob(const iEntityStats& stats, const Vec2& pos, bool isNorth)
    : Entity(stats, pos, isNorth)
    , m_pWaypoint(NULL)
{
    assert(dynamic_cast<const iEntityStats_Mob*>(&stats) != NULL);
}

void Mob::tick(float deltaTSec)
{
    // Tick the entity first.  This will pick our target, and attack it if it's in range.
    Entity::tick(deltaTSec);

    // if our target isn't in range, move towards it.
    if (!targetInRange())
    {
        move(deltaTSec);
    }
    // PROJECT 2: This is where your collision code will be called from
    std::vector<Entity*> otherMob = checkCollision();
    if (!otherMob.empty() || m_bInRiver) {
        processCollision(otherMob, deltaTSec);
    }
}

void Mob::move(float deltaTSec)
{
    // If we have a target and it's on the same side of the river, we move towards it.
    // Otherwise, we move toward the bridge.
    bool bMoveToTarget = false;
    if (!!m_pTarget)
    {    
        bool imTop = m_Pos.y < (GAME_GRID_HEIGHT / 2);
        bool otherTop = m_pTarget->getPosition().y < (GAME_GRID_HEIGHT / 2);

        if (imTop == otherTop)
        {
            bMoveToTarget = true;
        }
    }

    Vec2 destPos;
    if (bMoveToTarget)
    { 
        m_pWaypoint = NULL;
        destPos = m_pTarget->getPosition();
    }
    else
    {
        if (!m_pWaypoint)
        {
            m_pWaypoint = pickWaypoint();
        }
        destPos = m_pWaypoint ? *m_pWaypoint : m_Pos;
    }

    // Actually do the moving
    m_MoveVec = destPos - m_Pos;
    float distRemaining = m_MoveVec.normalize();
    float moveDist = m_Stats.getSpeed() * deltaTSec;

    // if we're moving to m_pTarget, don't move into it
    if (bMoveToTarget)
    {
        assert(m_pTarget);
        distRemaining -= (m_Stats.getSize() + m_pTarget->getStats().getSize()) / 2.f;
        distRemaining = std::max(0.f, distRemaining);
    }

    if (moveDist <= distRemaining)
    {
        m_Pos += m_MoveVec * moveDist;
    }
    else
    {
        m_Pos += m_MoveVec * distRemaining;

        // if the destination was a waypoint, find the next one and continue movement
        if (m_pWaypoint)
        {
            m_pWaypoint = pickWaypoint();
            destPos = m_pWaypoint ? *m_pWaypoint : m_Pos;
            m_MoveVec = destPos - m_Pos;
            m_MoveVec.normalize();
            m_Pos += m_MoveVec * distRemaining;
        }
    }

    
}

const Vec2* Mob::pickWaypoint()
{
    float smallestDistSq = FLT_MAX;
    const Vec2* pClosest = NULL;

    for (const Vec2& pt : Game::get().getWaypoints())
    {
        // Filter out any waypoints that are behind (or barely in front of) us.
        // NOTE: (0, 0) is the top left corner of the screen
        float yOffset = pt.y - m_Pos.y;
        if ((m_bNorth && (yOffset < 1.f)) ||
            (!m_bNorth && (yOffset > -1.f)))
        {
            continue;
        }

        float distSq = m_Pos.distSqr(pt);
        if (distSq < smallestDistSq) {
            smallestDistSq = distSq;
            pClosest = &pt;
        }
    }

    return pClosest;
}
// a simple Separating Axis Theorem system
bool Mob::collideWith(const Entity* self, const Entity* other) {

    float selfSizeHalf = self->getStats().getSize() / 2;
    float otherSizeHalf = other->getStats().getSize() / 2;

    float selfLeft      = self->getPosition().x - selfSizeHalf;
    float selfRight     = self->getPosition().x + selfSizeHalf;
    float selfTop       = self->getPosition().y - selfSizeHalf;
    float selfBottom    = self->getPosition().y + selfSizeHalf;

    float otherLeft     = other->getPosition().x - otherSizeHalf;
    float otherRight    = other->getPosition().x + otherSizeHalf;
    float otherTop      = other->getPosition().y - otherSizeHalf;
    float otherBottom   = other->getPosition().y + otherSizeHalf;

    if (selfLeft >= otherRight)     return false;
    if (selfRight <= otherLeft)     return false;
    if (selfTop >= otherBottom)     return false;
    if (selfBottom <= otherTop)     return false;

    return true;
}


// PROJECT 2: 
//  1) return a vector of mobs that we're colliding with
//  2) handle collision with towers & river 
std::vector<Entity*> Mob::checkCollision()
{   
    std::vector<Entity*> collideMobs;
    const Player& northPlayer = Game::get().getPlayer(true);
    for (Entity* pOtherMob : northPlayer.getMobs())
    {
        // don't collide with the mob itself
        if (this == pOtherMob) 
        {
            //std::cout << "self skipped" << std::endl;
            continue;
        }
        if (collideWith(this, pOtherMob)) {
            collideMobs.push_back(pOtherMob);
            //pOtherMob->takeDamage(5000);
        }
        // PROJECT 2: YOUR CODE CHECKING FOR A COLLISION GOES HERE
    }

    const Player& southPlayer = Game::get().getPlayer(false);
    for (Entity* pOtherMob : southPlayer.getMobs())
    {
        if (this == pOtherMob)
        {
            continue;
        }
        if (collideWith(this, pOtherMob)) {
            collideMobs.push_back(pOtherMob);
        }
    }
    // record if the curr mob is colliding with building/water
    m_bHitWall = false;
    for (Entity* pBuilding : northPlayer.getBuildings())
    {
        if (collideWith(this, pBuilding)) {
            std::cout << "||north building " << pBuilding->getStats().getName() << std::endl;
            collideMobs.push_back(pBuilding);
            m_WallPos = pBuilding->getPosition();
            m_bHitWall = true;
        }
    }

    for (Entity* pBuilding : southPlayer.getBuildings())
    {
        if (collideWith(this, pBuilding)) {
            collideMobs.push_back(pBuilding);
            m_WallPos = pBuilding->getPosition();
            m_bHitWall = true;
        }
    }

    m_bInRiver = false;
    float selfSizeHalf = this->getStats().getSize() / 2;
    Vec2 selfPos = this->getPosition();
    // if mob is within river y range
    if (selfPos.y + selfSizeHalf > RIVER_TOP_Y && selfPos.y - selfSizeHalf < RIVER_BOT_Y) {
        // if the mob is not on brige
        if (std::abs(selfPos.x - LEFT_BRIDGE_CENTER_X) > BRIDGE_WIDTH/2 - selfSizeHalf &&
            std::abs(selfPos.x - RIGHT_BRIDGE_CENTER_X) > BRIDGE_WIDTH/2 - selfSizeHalf) {
            m_bInRiver = true;
            // river is also an unpushable object
            m_bHitWall = true;
        }
    }
    return collideMobs;
}

void Mob::processCollision(std::vector<Entity*> otherMob, float deltaTSec)
{
    float diffX, diffY, contactDirection;
    float horizontalShift, verticalShift;
    float moveAroundSpeed;
    // as all units are squares, we use hard coded 45 degree angle (Dy / Dx = 1) as the reference to tell contact direction
    for (Entity *pOtherMob : otherMob) {
        // determine which direction does the collision happen
        diffX = std::abs(pOtherMob->getPosition().x - this->getPosition().x);
        diffY = std::abs(pOtherMob->getPosition().y - this->getPosition().y);

        if (diffX > 0.001f) contactDirection = diffY / diffX;
        else contactDirection = 2; // just assign a value larger than 1
        // the lighter get pushed
        // isHittingWall
        Entity* bePushed = pOtherMob->getStats().getMass() > this->getStats().getMass() ? this : pOtherMob;
        Entity* pushing  = bePushed == pOtherMob ? this : pOtherMob;
        std::string pushingSide = pushing->isNorth() ? "North" : "South";
        std::string bePushedSide = bePushed->isNorth() ? "North" : "South";

        moveAroundSpeed = bePushed->getStats().getSpeed() * deltaTSec / GET_AROUND_MOVE;
        if (moveAroundSpeed < 0.1f) moveAroundSpeed = 0.12f;
        
        if (contactDirection > DIAGONAL_ANGLE) { // this means contact at top/bottom
            // always be pushed to the right - Avoid infinite/stasis collision when same mass
            horizontalShift = bePushed->getPosition().x < pushing->getPosition().x ? -moveAroundSpeed : moveAroundSpeed;
            // if the be pushed on top (smaller y), push back (smallllller y) else bottom (larger y -> lagggggger y)
            verticalShift = (bePushed->getStats().getSize() + pushing->getStats().getSize()) / 2 - diffY; // get penetration
            //verticalShift += CONTACT_BOUNCE;
            verticalShift = bePushed->getPosition().y < pushing->getPosition().y ? -verticalShift : verticalShift;
        }
        else if (contactDirection < DIAGONAL_ANGLE) { // this means contact at left/right
            // if the being pushed is on the left of the pushing unit, move back left
            horizontalShift = (bePushed->getStats().getSize() + pushing->getStats().getSize()) / 2 - diffX; // get penetration
            horizontalShift = bePushed->getPosition().x < pushing->getPosition().x ? -horizontalShift : horizontalShift;
            verticalShift = bePushed->isNorth() ? moveAroundSpeed : -moveAroundSpeed;
        }
        // all none building units should not push an unit that is currently hitting building
        if (bePushed->isHittingWall() && pushing->getStats().getMass() < FLT_MAX) {
            // update the relative pos between bepushed (on wall) and wall, this is for correctly update the pushing obj
            diffX = std::abs(bePushed->getPosition().x - bePushed->getWallPosition().x);
            diffY = std::abs(bePushed->getPosition().y - bePushed->getWallPosition().y);
            if (diffX > 0.001f) contactDirection = diffY / diffX;
            else contactDirection = 2;
            // the original pushing unit should try to avoid pushing target
            if (contactDirection > DIAGONAL_ANGLE) {
                bePushed->bePushedAway(horizontalShift, 0.f);
                verticalShift = bePushed->getPosition().y < pushing->getPosition().y ? moveAroundSpeed : -moveAroundSpeed;
                pushing->bePushedAway(-horizontalShift, verticalShift);
            }
            else {
                bePushed->bePushedAway(0.f, verticalShift);
                horizontalShift = bePushed->getPosition().x < pushing->getPosition().x ? moveAroundSpeed : -moveAroundSpeed;
                pushing->bePushedAway(horizontalShift, -verticalShift);
            }
        }
        
        else {
            // when same masses, both push each other
            if (pushing->getStats().getMass() == bePushed->getStats().getMass()) {
                horizontalShift = contactDirection > DIAGONAL_ANGLE  ? horizontalShift / 2 : horizontalShift;
                verticalShift = contactDirection > DIAGONAL_ANGLE ? verticalShift : verticalShift / 2;
                pushing->bePushedAway(-horizontalShift, -verticalShift);
            }
            // execute the pushing
            bePushed->bePushedAway(horizontalShift, verticalShift);
        }
    }
    
    // process river collision
    if (m_bInRiver) {
        moveAroundSpeed = this->getStats().getSpeed() * deltaTSec / GET_AROUND_MOVE;
        float selfSizeHalf = this->getStats().getSize() / 2;
        Vec2 selfPos = this->getPosition();
        // push up/down if not in bridge range
        if (this->getMoveVec().y > 0) { // moving down/south
            verticalShift = selfPos.y + selfSizeHalf - RIVER_TOP_Y;
        }
        else {
            verticalShift = RIVER_BOT_Y - (selfPos.y - selfSizeHalf);
        }
        // compensate the penetration
        this->bePushedAway(0.f, verticalShift);
    }
}


