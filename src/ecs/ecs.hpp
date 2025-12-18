#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

///

struct IComponent
{
protected:
    static int nextId;
};

template <typename T>
class Component : IComponent
{
public:
    static int getId() 
    { 
        static auto id = nextId++; 
        return id;
    };
};

enum class Tag
{
    PLAYER = 0,
    OBJECT,
    ENEMY,
    TERRAIN
};


///
class Entity
{
public:
    Entity(int id) : _id(id) {};
    Entity(const Entity& entity) = default;
    int getId() const;

    void tag(Tag tag);
    bool hsTag(Tag tag) const;
    void group(Tag group);
    void belongToGroup(Tag group) const;

    template <typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
    template <typename TComponent> void removeComponent();
    template <typename TComponent> bool hasComponent() const;
    template <typename TComponent> TComponent& getComponent() const;

    Entity& operator = (const Entity& other) = default;

    bool operator ==(const Entity& other) const
    {
        return _id == other._id;
    }

    bool operator !=(const Entity& other) const
    {
        return _id != other._id;
    }

    bool operator <(const Entity& other) const
    {
        return _id < other._id;
    }

    bool operator >(const Entity& other) const
    {
        return _id > other._id;
    }

    void kill();

    class Registry* registry;

private:
    int _id;
};

template <typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args)
{
    registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::removeComponent()
{
    registry->removeComponent<TComponent>(*this);

}

template <typename TComponent>
bool Entity::hasComponent() const
{
    return registry->hasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::getComponent() const
{
    return registry->getComponent<TComponent>(*this);
}

/// process entities with given signature
class System{
public:
    System() = default;
    ~System() = default;

    void addEntityToSystem(Entity entity);
    void removeEntityFromSystem(Entity entity);
    std::vector<Entity> getSystemEntities() const;
    const Signature& getComponentSignature() const;

    template<typename TComponent> void requireComponent();

private:
    Signature _componentSignature;
    std::vector<Entity> _entities;

};

template<typename TComponent>
void System::requireComponent()
{
    const auto componentId = Component<TComponent>::getId();
    _componentSignature.set(componentId);
}


class IPool
{
public:
    virtual ~IPool() = default;
    virtual void removeEntityFromPool(int entityId) = 0;
};

template<typename T>
class Pool : public IPool
{
public:
    Pool(int capacity = 100)
    {
        size = 0;
        data.resize(capacity);
    }

    virtual ~Pool() = default;

    bool isEmpty() const { return size == 0; }

    int getSize() const { return size; }

    void resize(int n) { data.resize(n); }

    void clear() { 
        data.clear();
        size = 0;
    }

    void add(T object)
    {
        data.push_back(object);
    }

    void set(int entityId, T object)
    { 
        if(enitityIdToIndex.find(entityId) != enitityIdToIndex.end())
        {
            int index  = enitityIdToIndex[entityId];
            data[index] = object;
        }
        else
        {
            int index = size;
            enitityIdToIndex.emplace(entityId, index);
            indexToEntityId.emplace(index, entityId);
            if(index >= data.capacity())
            {
                data.resize(size * 2);
            }
            data[index] = object; 
            size++;
        }
    }

    void remove(int entityId)
    {
        int indexOfRemoved = enitityIdToIndex[entityId];
        int indexOfLast = size - 1;
        data[indexOfRemoved] = data[indexOfLast];

        int entityIdOfLastElement = indexToEntityId[indexOfLast];
        enitityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
        indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

        enitityIdToIndex.erase(entityId);
        indexToEntityId.erase(indexOfLast);
        
        size--;
    }
    
    void removeEntityFromPool(int entityId) override
    {
        if(enitityIdToIndex.find(entityId) != enitityIdToIndex.end())
        {
            remove(entityId);
        }
    }

    T& get(int entityId) {
        int index = enitityIdToIndex[entityId];
        return static_cast<T&>(data.at(index)); 
    }

    T& operator [](unsigned int index) { return data[index]; }
private:
    std::vector<T> data;
    int size;

    std::unordered_map<int, int> enitityIdToIndex;
    std::unordered_map<int, int> indexToEntityId;
};

///
class Registry
{
public:
    Registry() = default;
    ~Registry() = default;

    Entity createEntity();
    void addEntityToSystem(Entity entity);
    void killEntity(Entity entity);

    template<typename TComponent, typename ...Targs> void addComponent(Entity entity, Targs&& ...args);
    template<typename TComponent> void removeComponent(Entity entity);
    template<typename TComponent> bool hasComponent(Entity entity) const;
    template<typename TComponent> TComponent& getComponent(Entity entity) const;
    
    template<typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
    template<typename TSystem> void removeSystem();
    template<typename TSystem> bool hasSystem() const ;
    template<typename TSystem> TSystem& getSystem() const;

    void addEntityToSystems(Entity entity);
    void removeEntityFromSystems(Entity entity);

    void update();

    void tagEntity(Entity entity, Tag tag);
    bool entityHasTag(Entity entity, Tag tag) const;
    Entity getEntityByTag(Tag tag) const;
    void removeEntityTag(Entity entity);
    
    void groupEntity(Entity entity, Tag group);
    bool entityBelongsToGroup(Entity entity, Tag group) const;
    std::vector<Entity> getEntitiesByGroup(Tag tag) const;
    void removeEntityGroup(Entity entity);

private:
    int _numEntities = 0;

    // vector index = component type id
    // Pool index = entity id
    std::vector<std::shared_ptr<IPool>> _componentPools;

    // vector index =  entity id;
    std::vector<Signature> _entityComponentSignatures;

    //
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems;

    std::set<Entity> _entitiesToBeAdded;
    std::set<Entity> _entitiesToBeKilled;

    std::deque<int> freeIds;

    std::unordered_map<Tag, Entity> entityPerTag;
    std::unordered_map<int, Tag> tagPerEntity;

    std::unordered_map<Tag, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<int, Tag> groupPerEntity;

};

template<typename TSystem> 
TSystem& Registry::getSystem() const 
{
    auto system = _systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template<typename TSystem> 
bool Registry::hasSystem() const 
{
    return _systems.find(std::type_index(typeid(TSystem))) != _systems.end();
}

template<typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    _systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::removeSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    _systems.erase(system);
}

template<typename TComponent>
bool Registry::hasComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    return _entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent>
TComponent& Registry::getComponent(Entity entity) const
{
    auto componentId = Component<TComponent>::getId();
    auto entityId = entity.getId();

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(_componentPools[componentId]);
    return componentPool->get(entityId);
}

template<typename TComponent, typename ...TArgs>
void Registry::addComponent(Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    if(componentId >= _componentPools.size())
    {
        _componentPools.resize(componentId + 1, nullptr);
    }

    if(!_componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        _componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(_componentPools[componentId]);

    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->set(entityId, newComponent);
    _entityComponentSignatures[entityId].set(componentId);

    Logger::log("component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template<typename TComponent>
void Registry::removeComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    _entityComponentSignatures[entityId].set(componentId, false);
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(_componentPools[componentId]);
    componentPool->remove(entityId);
    Logger::log("component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

#endif