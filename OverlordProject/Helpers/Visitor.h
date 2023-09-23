#pragma once

#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/PowerUps/PowerUp.h"
#include "Prefabs/RockPrefab.h"

template<typename... Args>
class Visitor
{
public:
    virtual ~Visitor() = default;

    template<typename... Args>
    void Visit(GameObject* object, Args... args)
    {
        if (auto bomberman = dynamic_cast<BombermanCharacter*>(object))
        {
            Visit(bomberman, args...);
        }
        else if (auto powerUp = dynamic_cast<BasePowerUp*>(object))
        {
            Visit(powerUp, args...);
        }
        else if (auto rock = dynamic_cast<RockPrefab*>(object))
        {
            Visit(rock, args...);
        }
    }

    virtual void Visit(BombermanCharacter*, Args...) {}
    virtual void Visit(BasePowerUp*, Args...) {}
    virtual void Visit(RockPrefab*, Args...) {}

};