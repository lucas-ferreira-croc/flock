#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

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
    static int getId() 
    { 
        static auto id = nextId++; 
        return id;
    };
};

///
class Entity
{
public:
    Entity(int id) : _id(id) {};
    int getId() const;

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
private:
    int _id;
};

/// process entities with given signature
class System{
public:
    System() = default;
    virtual ~System() = default;

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
    virtual ~IPool(){}
};

template<typename T>
class Pool : IPool
{
public:
    Pool(int size = 100)
    {
        data.rezie(size);
    }

    virtual ~Pool() = default;

    bool isEmpty() const { return data.empty(); }

    int getSize() const { return data.size(); }

    void resize(int n) { data.resize(n); }

    void clear() { data.clear(); }

    void add(T object) { data.push_back(object); }

    void set(int index, T object) { data[index] = object; }

    T& get(int index) { return static_cast<T>(data.at(index)); }

    T& operator [](unsigned int index) { return data[index]; }
private:
    std::vector<T> data;
};

///
class Registry
{
public:
    Registry();

    Entity createEntity();
    void addEntityToSystem(Entity entity);
    //killEntity();

    template<typename TComponent, typename ...Targs> void addComponent(Entity entity, Targs&& ...args);
    template<typename TComponent> void removeComponent(Entity entity);
    template<typename TComponent> bool hasComponent(Entity entity);
    //getComponent(Entity entity);
    
    template<typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
    template<typename TSystem> void removeSystem();
    template<typename TSystem> bool hasSystem() const ;
    template<typename TSystem> TSystem& getSystem() const;

    void addEntityToSystems(Entity entity);
    
    void update();
private:
    int _numEntities = 0;

    // vector index = component type id
    // Pool index = entity id
    std::vector<IPool*> _componentPools;

    // vector index =  entity id;
    std::vector<Signature> _entityComponentSignatures;

    //
    std::unordered_map<std::type_index, System*> _systems;

    std::set<Entity> _entitiesToBeAdded;
    std::set<Entity> _entitiesToBeKilled;
};

template<typename TSystem> 
TSystem& Registry::getSystem() const 
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *std::static_cast<TSystem>(system->second);
}

template<typename TSystem> 
bool Registry::hasSystem() const 
{
    return _systems.find(std::type_index(typeid(TSystem))) != _systems.end();
}

template<typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args)
{
    TSystem* newSystem = new System(std::forward<TArgs>(args)...);
    _systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::removeSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    _systems.erase(system);
}

template<typename TComponent>
bool Registry::hasComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    return _entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent, typename ...TArgs>
void Registry::addComponent(Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    if(componentId >= _componentPools.size())
    {
        _componentPools.resize(componentId + 1, nulltr);
    }

    if(!_componentPools[componentId])
    {
        Pool<TComponent>* newComponentPool = new Pool<TComponent>();
        _componentPools[componentId] = newComponentPool;
    }

    Pool<T>* componentPool = Poolt<TComponent>(_componentPools<TComponent>[componentId]);

    if(entityId >= componentPool->getSize())
    {
        componentPool->resize(_numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->set(entityId, newComponent);
    _entityComponentSignatures[entityId].set(componentId);
}

template<typename TComponent>
void Registry::removeComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    _entityComponentSignatures[entity].set(componentId, false);
}

#endif