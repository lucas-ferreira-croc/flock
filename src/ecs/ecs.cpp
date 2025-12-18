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

void Entity::tag(Tag tag)
{
    registry->tagEntity(*this, tag);
}

bool Entity::hsTag(Tag tag) const 
{
    return registry->entityHasTag(*this, tag);
}

void Entity::group(Tag group)
{
    registry->groupEntity(*this, group);
}

void Entity::belongToGroup(Tag group) const 
{
    registry->entityBelongsToGroup(*this, group);
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

void Registry::tagEntity(Entity entity, Tag tag)
{
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.getId(), tag);
}

bool Registry::entityHasTag(Entity entity, Tag tag) const
{
   if(tagPerEntity.find(entity.getId()) == tagPerEntity.end())
   {
        return false;
   }

   return entityPerTag.find(tag)->second == entity;
}

Entity Registry::getEntityByTag(Tag tag) const
{
    return entityPerTag.at(tag);
}

void Registry::removeEntityTag(Entity entity)
{
    auto taggedEntity = tagPerEntity.find(entity.getId());
    if(taggedEntity != tagPerEntity.end())
    {
        auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::groupEntity(Entity entity, Tag group)
{
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.getId(), group);
}

bool Registry::entityBelongsToGroup(Entity entity, Tag group) const
{
    auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity.getId()) != groupEntities.end();
}

std::vector<Entity> Registry::getEntitiesByGroup(Tag tag) const
{
    auto& setOfEntities = entitiesPerGroup.at(tag);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void Registry::removeEntityGroup(Entity entity)
{
    auto groupedEntity = groupPerEntity.find(entity.getId());
    if(groupedEntity != groupPerEntity.end())
    {
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if(group != entitiesPerGroup.end())
        {
            auto entityInGroup = group->second.find(entity);
            if(entityInGroup != group->second.end())
            {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
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

        removeEntityTag(entity);
        removeEntityGroup(entity);
    }
    _entitiesToBeKilled.clear();
}