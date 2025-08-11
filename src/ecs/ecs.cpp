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
    int entityId = _numEntities++;
    Entity entity(entityId);
    entity.registry = this;

    _entitiesToBeAdded.insert(entity);
    if(entityId >= _entityComponentSignatures.size())
    {
        _entityComponentSignatures.resize(entityId + 1);
    }
    Logger::log("Entity created with id = " + std::to_string(entityId));
    return entity;
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

void Registry::update()
{
    for(auto entity : _entitiesToBeAdded)
    {
        addEntityToSystems(entity);
    }
    _entitiesToBeAdded.clear();
}