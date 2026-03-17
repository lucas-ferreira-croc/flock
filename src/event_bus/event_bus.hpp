#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include <typeindex>
#include <map>
#include<memory>
#include <list>
#include "event.hpp"
#include <functional>

class IEventCallBack
{
public:
    virtual ~IEventCallBack() = default;
    void execute(Event& e)
    {
        call(e);
    }
private:
    virtual void call(Event& e) = 0;
};

template <typename TOwner, typename TEvent>
class EventCallback  : public IEventCallBack
{
private:
    typedef void (TOwner::*CallbackFunction)(TEvent&);
    TOwner* ownerInstance;
    CallbackFunction callbackFunction;
    virtual void call(Event& e) override 
    {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
    }

    
public:
    EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
    {
        this->ownerInstance = ownerInstance;
        this->callbackFunction = callbackFunction;
    }
    
    virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallBack>> HandlerList;

class EventBus
{
public:
    EventBus() 
    {
        
    }

    ~EventBus()
    {
        
    }

    void reset()
    {
        subscribers.clear();
    }

    template<typename TEvent, typename TOwner>
    void subscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&))
    {
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        if(!subscribers[typeid(TEvent)].get())
        {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }

    template<typename TEvent, typename ...TArgs>
    void emmitEvent(TArgs&& ...args)
    {
        auto handlers = subscribers[typeid(TEvent)].get();
        if(handlers)
        {
            for(auto it = handlers->begin(); it != handlers->end(); it++)
            {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->execute(event);
            }
        }
    }
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
};

#endif
