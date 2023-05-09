#include "worldbase.h"

WorldBase::~WorldBase() {
	for (auto& e : entities) e.second->clearRelatedSystems();
}


Entity& getIth(std::unordered_map<int, std::unique_ptr<Entity>>& list, int i) {
    return *list.at(i);
}

std::function<Entity*(EntityID)> WorldBase::getIDToEntityFunc() {
    return [=] (EntityID ID) -> Entity* {
        if (entities.count(ID) == 0) return nullptr;
        return entities.at(ID).get();
    };
}


EntityID WorldBase::makeEntity(const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p) {
    EntityID ID = makeNewID();

    _makeEntity(ID, componentList, p);

    return ID;
}


EntityID WorldBase::makeEntity(const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p) {
    EntityID ID = makeNewID();

    _makeEntity(ID, componentList, p);

    return ID;
}


void WorldBase::_makeEntity(EntityID ID, const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p) {
    std::unique_ptr<Entity> e = std::make_unique<Entity>(ID, p, componentList, getTypeToSystemFunc());

    entities.insert(std::make_pair<EntityID, std::unique_ptr<Entity>>(EntityID(ID), std::move(e)));

    for (auto& r : getEntity(ID).getRelatedSystems()) r->postCreate(ID);
}


void WorldBase::_makeEntity(EntityID ID, const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p) {
    std::unique_ptr<Entity> e = std::make_unique<Entity>(ID, p, componentList, getTypeToSystemFunc());

    entities.insert(std::make_pair<EntityID, std::unique_ptr<Entity>>(EntityID(ID), std::move(e)));

    for (auto& r : getEntity(ID).getRelatedSystems()) r->postCreate(ID);
}

EntityID WorldBase::makeEntityNextFrame(std::vector<std::unique_ptr<IPartialComponent>>&& list) {
    EntityID ID = makeNewID();

    creationQueue.emplace(ID, std::move(list));

    return ID;
}

void WorldBase::deleteEntityNextFrame(EntityID ID) {
    deletionQueue.push_back(ID);
}

Entity& WorldBase::getEntity(EntityID i) {
    return *entities.at(i);
}

std::function<SystemBase&(SystemType)> WorldBase::getTypeToSystemFunc() {
    std::unordered_map<SystemType, SystemBase&>& map = typeToSystem;
    return [=] (SystemType st) -> SystemBase& {
        return map.at(st);
    };
}


void WorldBase::update(double deltaTime) {
    //delete all entities flagged for deletion
    for (auto& i : deletionQueue) entities.erase(i);
    deletionQueue.clear();

    //create all new entities
    for (auto& i : creationQueue) {
        EntityID ID = i.first;
        std::vector<std::reference_wrapper<IPartialComponent>> inList;

        for (auto& j : i.second) {
            assert(j.get() != nullptr);
            inList.push_back(*j);
        }

        _makeEntity(ID, inList, Placement());
    }
    creationQueue.clear();

    for (auto& e : entities) e.second->updatePrevPos(deltaTime);

    customUpdate(deltaTime);

}


