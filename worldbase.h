#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "component.h"
#include "actor.h"

#include <type_traits>


template <class T>
struct ConcatWrapper;



class WorldBase {
    public:
    WorldBase(std::vector<std::reference_wrapper<SystemBase>> systems);
    ~WorldBase();


    void update(double deltaTime);

    void deleteEntityNextFrame(EntityID ID);

    ///prefer using make/deleteEntityNextGrame, to limit inter-system update order dependence
    EntityID makeEntityNextFrame(std::vector<std::unique_ptr<IPartialComponent>>&& list);
    EntityID makeEntityNextFrame(std::vector<std::shared_ptr<IPartialComponent>> list);

    //probably avoid using makeEntityRefList outside of temporary debug stuff
    EntityID makeEntityRefList(const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p = Placement());
    EntityID makeEntity(const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p = Placement());

    ///should there be an appendComponentNextFrame()?
    void appendComponent(EntityID eid, const IPartialComponent& pc);
    void appendComponent(EntityID eid, std::shared_ptr<IPartialComponent> pc);

    void deleteEntity(EntityID eid);

    ///variadic makeEntity functions:
    /// accepts shared_ptr, vector<shared_ptr>, raw ptrs of IPartialComponent
    ///note: high probability that these end up being unacceptably slow for some use cases,
    /// since I'm not entirely sure what the templating evaluates to
    template<class ...Args>
    EntityID makeEntityNextFrame(Placement p, Args... args);

    template<class ...Args>
    EntityID makeEntity(Placement p, Args... args);

    Entity& getEntity(EntityID i);
    bool hasEntity(EntityID i) { return entities.count(i) > 0; }

    ///this will always save entities in the order of the input list
    std::vector<SavedEntity> saveEntities(std::vector<EntityID> eids);


    //load entity: copy 1-1 into this world
    // good for loading, bad for creating entities (ID collisions between strong references, maintains weak references)
    EntityID loadEntity(const SavedEntity& e);
    std::vector<EntityID> loadEntities(const std::vector<SavedEntity>& list);

    //duplicate entity: copy entity into new world; templates change based on system-specified behavior
    // ex: strong references remap, weak references are broken
    EntityID duplicateEntity(const SavedEntity& e);
    std::vector<EntityID> duplicateEntities(const std::vector<SavedEntity>& list);

    SystemBase* getSystemIndirect(SystemType t);

    private:
    int maxID;

    std::unordered_map<SystemType, SystemBase&> typeToSystem;
    std::unordered_map<EntityID, std::vector<std::unique_ptr<IPartialComponent>>> creationQueue;
    std::vector<EntityID> deletionQueue;

    std::function<SystemBase&(SystemType)> getTypeToSystemFunc();

    std::vector<std::reference_wrapper<SystemBase>> knownSystems;

    //since types aren't known at ctor time (supertype created before base type), this generates typeToSystem from knownSystems
    void constructSystemTypemap();

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

template <class T, class K>
constexpr void assertConcatTypeMatch(K thing) {
    static_assert(std::is_constructible<ConcatWrapper<T>, K>(), "Concatenation type mismatch; an element couldn't be converted to ConcatWrapper<T>");
}

template <class T, class K, class... Args>
constexpr void assertConcatTypeMatch(K thing, Args... args) {
    static_assert(std::is_constructible<ConcatWrapper<T>, K>(), "Concatenation type mismatch; an element couldn't be converted to ConcatWrapper<T>");

    assertConcatTypeMatch<T>(args...);
}

template<class T>
constexpr void recursiveConcat(std::vector<std::shared_ptr<T>>& outList, ConcatWrapper<IPartialComponent> i) {
    outList.insert(outList.end(), i.list.begin(), i.list.end());
}

template<class T, class... Args>
constexpr void recursiveConcat(std::vector<std::shared_ptr<T>>& outList, ConcatWrapper<IPartialComponent> i, Args... args) {

    outList.insert(outList.end(), i.list.begin(), i.list.end());

    recursiveConcat(outList, args...);
}

template <class T, class... Args>
constexpr const std::vector<std::shared_ptr<T>> baseConcatenate(Args... args) {
    std::vector<std::shared_ptr<T>> out;

    recursiveConcat(out, args...);

    return out;
}

template <class... Args>
constexpr const std::vector<std::shared_ptr<IPartialComponent>> concatenate(Args... args) {
    assertConcatTypeMatch<IPartialComponent>(args...);

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
