#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "component.h"
#include "actor.h"


template <class T>
struct ConcatWrapper;

class WorldBase {
    public:
    WorldBase(std::unordered_map<SystemType, SystemBase&> typeToSystemMap)
        : typeToSystem(typeToSystemMap), maxID(0) {};
	virtual ~WorldBase();

    void update(double deltaTime);


    void deleteEntityNextFrame(EntityID ID);
    EntityID makeEntityNextFrame(std::vector<std::unique_ptr<IPartialComponent>>&& list);
    EntityID makeEntityNextFrame(std::vector<std::shared_ptr<IPartialComponent>> list);

    EntityID makeEntity(const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p = Placement());
    EntityID makeEntity(const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p = Placement());



    ///variadic makeEntity functions:
    /// accepts shared_ptr, vector<shared_ptr>, raw ptrs of IPartialComponent
    ///note: high probability that these end up being unacceptably slow for some use cases, with how the impl works
    template<class ...Args>
    EntityID makeEntityNextFrame(Placement p, Args... args);

    template<class ...Args>
    EntityID makeEntity(Placement p, Args... args);

    Entity& getEntity(EntityID i);
    bool hasEntity(EntityID i) { return entities.count(i) > 0; }


    private:
    int maxID;

    std::unordered_map<SystemType, SystemBase&> typeToSystem;
    std::unordered_map<EntityID, std::vector<std::unique_ptr<IPartialComponent>>> creationQueue;
    std::vector<EntityID> deletionQueue;

    std::function<SystemBase&(SystemType)> getTypeToSystemFunc();

    protected:

    virtual void customUpdate(double deltaTime) =0;

    EntityID makeNewID() { return EntityID(maxID++); }
    void _makeEntity(EntityID ID, const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p);
    void _makeEntity(EntityID ID, const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p);

    std::function<Entity*(EntityID)> getIDToEntityFunc();

    ///maybe limit how derived types can modify this?
    std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;
};


///this stuff has to pollute the header for the compiler to understand the templating

template <class T>
struct ConcatWrapper {
    std::vector<std::shared_ptr<T>> list;

    ///intentional implicit conversion
    ConcatWrapper(const std::shared_ptr<T>& obj) {
        list.push_back(obj);
    }

    ///intentional implicit conversion
    ConcatWrapper(T* obj) {
        list.push_back(std::shared_ptr<T>(obj));
    }

    ///intentional implicit conversion
    ConcatWrapper(const std::vector<std::shared_ptr<T>>& obj)
        : list(obj) {}

    ///intentional implicit conversion
    ConcatWrapper(std::vector<std::shared_ptr<T>>&& obj)
        : list(std::move(obj)) {}
};

template<class T>
void recursiveConcat(std::vector<std::shared_ptr<T>>& outList, ConcatWrapper<IPartialComponent> i) {
    outList.insert(outList.end(), i.list.begin(), i.list.end());
}

template<class T, class... Args>
void recursiveConcat(std::vector<std::shared_ptr<T>>& outList, ConcatWrapper<IPartialComponent> i, Args... args) {
    outList.insert(outList.end(), i.list.begin(), i.list.end());

    recursiveConcat(outList, args...);
}

template <class T, class... Args>
const std::vector<std::shared_ptr<T>> baseConcatenate(Args... args) {
    std::vector<std::shared_ptr<T>> out;

    recursiveConcat(out, args...);

    return out;
}

template <class... Args>
const std::vector<std::shared_ptr<IPartialComponent>> concatenate(Args... args) {
    return baseConcatenate<IPartialComponent>(args...);
}


template<class ...Args>
EntityID WorldBase::makeEntityNextFrame(Placement p, Args... args) {
    assert(false && "makeEntityNextFrame doesn't accept Placements yet");
    return makeEntityNextFrame(concatenate(args...));
}


template<class ...Args>
EntityID WorldBase::makeEntity(Placement p, Args... args) {
    return makeEntity(concatenate(args...), p);
}
