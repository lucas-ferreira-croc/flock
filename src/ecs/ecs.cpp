#include "ecs.hpp"
#include "log/logger.hpp"
#include <algorithm>

int IComponent::nextId = 0;

int Entity::getId() const
{
    return _id;
}

void System::addEntityToSystem(Entity entity)
{
    _entities.push_back(entity);
}

void System::removeEntityFromSystem(Entity entity)
{
    _entities.erase(
        std::remove_if(_entities.begin(), _entities.end(), [&entity](Entity other)
        {
            return entity == other;
        }),
    _entities.end());
}

std::vector<Entity> System::getSystemEntities() const 
{
    return _entities;
}

const Signature& System::getComponentSignature() const
{
    return _componentSignature;
}

Entity Registry::createEntity()
{
    int entityId;
    if(freeIds.empty())
    {
        entityId = _numEntities++;
        if(entityId >= _entityComponentSignatures.size())
        {
            _entityComponentSignatures.resize(entityId + 1);
        }
    }
    else
    {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;

    _entitiesToBeAdded.insert(entity);
    Logger::log("Entity created with id = " + std::to_string(entityId));
    return entity;
}

void Entity::kill()
{
    registry->killEntity(*this);
}

void Registry::killEntity(Entity entity)
{
    _entitiesToBeKilled.insert(entity);
}

void Registry::addEntityToSystems(Entity entity)
{
    const auto entityId = entity.getId();
    const auto& entityComponentSignature = _entityComponentSignatures[entityId];

    for(auto& system : _systems)
    {
        const auto& systemComponentSignature = system.second->getComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if(isInterested)
        {
            system.second->addEntityToSystem(entity);
        }
    }    
}

void Registry::removeEntityFromSystems(Entity entity)
{
    for(auto system : _systems)
    {
        system.second->removeEntityFromSystem(entity);
    }
}

void Registry::update()
{
    for(auto entity : _entitiesToBeAdded)
    {
        addEntityToSystems(entity);
    }
    _entitiesToBeAdded.clear();

    for(auto entity : _entitiesToBeKilled)
    {
        removeEntityFromSystems(entity);
        _entityComponentSignatures[entity.getId()].reset();

        for(auto pool : _componentPools)
        {
            if(pool)
            {
                pool->removeEntityFromPool(entity.getId());
            }
        }
        freeIds.push_back(entity.getId());

        // removeEntityTag(entity);
        // removeEntityGroup(entity);
    }
    _entitiesToBeKilled.clear();
}