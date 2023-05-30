#include "worldbase.h"


WorldBase::WorldBase(std::vector<std::reference_wrapper<SystemBase>> systems)
    : typeToSystem(), maxID(0), knownSystems(systems) {};
WorldBase::~WorldBase() {
    for (auto& e : entities) e.second->clearRelatedSystems();
}

void WorldBase::constructSystemTypemap() {
    for (auto& s : knownSystems) {
        SystemType type = s.get().getType();

        if (typeToSystem.count(type)) {
            throw std::invalid_argument("WorldBase recieved system list with Type duplicates");
        }

        typeToSystem.insert(std::pair<SystemType, SystemBase&>(type, s.get()));
    }

    knownSystems.clear();
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

void WorldBase::deleteEntity(EntityID eid) {
    assert(hasEntity(eid));
    entities.erase(eid);
}

EntityID WorldBase::makeEntityRefList(const std::vector<std::reference_wrapper<IPartialComponent>>& componentList, Placement p) {
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
    if (entities.count(ID)) throw std::invalid_argument("Tried to create an entity with an already existing EID");

    std::unique_ptr<Entity> e = std::make_unique<Entity>(ID, p, componentList, getTypeToSystemFunc());

    entities.insert(std::make_pair<EntityID, std::unique_ptr<Entity>>(EntityID(ID), std::move(e)));

    for (auto& r : getEntity(ID).getRelatedSystems()) r->postCreate(ID);
}


void WorldBase::_makeEntity(EntityID ID, const std::vector<std::shared_ptr<IPartialComponent>>& componentList, Placement p) {
    if (entities.count(ID)) throw std::invalid_argument("Tried to create an entity with an already existing EID");

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
    if (typeToSystem.size() == 0) constructSystemTypemap();

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


void WorldBase::appendComponent(EntityID eid, const IPartialComponent& pc) {
    Entity& e = getEntity(eid);

    auto t2s = getTypeToSystemFunc();

    SystemBase& sb = pc(eid, t2s);

    e.addRelatedSystem(&sb);
    sb.postCreate(eid);
}

void WorldBase::appendComponent(EntityID eid, std::shared_ptr<IPartialComponent> pc) {
    assert(pc.get() != nullptr);
    appendComponent(eid, *pc);
}

std::vector<SavedEntity> WorldBase::saveEntities(std::vector<EntityID> eids) {
    std::vector<SavedEntity> out;
    for (auto& e : eids) out.push_back(getEntity(e).save());
    return out;
}


EntityID WorldBase::loadEntity(const SavedEntity& e) {
    _makeEntity(e.ID, e.components, e.pos);
    return e.ID;
}

std::vector<EntityID> WorldBase::loadEntities(const std::vector<SavedEntity>& list) {
    std::vector<EntityID> out;
    for (auto& e : list) out.push_back(loadEntity(e));
    return out;
}

EntityID WorldBase::duplicateEntity(const SavedEntity& e) {
    std::unordered_map<EntityID, EntityID> mapping;

    EntityID outID = makeNewID();
    mapping.insert(std::make_pair(e.ID, outID));

    std::vector<std::shared_ptr<IPartialComponent>> modifiedComponents;

    for (auto& c : e.components) {
        std::shared_ptr<IPartialComponent> newPC = c->duplicateUpdate(mapping);
        if (newPC.get() != nullptr) modifiedComponents.push_back(newPC);
        else modifiedComponents.push_back(c);
    }

    _makeEntity(outID, modifiedComponents, e.pos);

    return outID;
}

std::vector<EntityID> WorldBase::duplicateEntities(const std::vector<SavedEntity>& list) {
    std::vector<EntityID> out;

    std::unordered_map<EntityID, EntityID> mapping;

    for (auto& se : list) mapping.insert(std::make_pair(se.ID,  makeNewID()));

    for (auto& se : list) {
        std::vector<std::shared_ptr<IPartialComponent>> modifiedComponents;

        for (auto& c : se.components) {
            std::shared_ptr<IPartialComponent> newPC = c->duplicateUpdate(mapping);
            if (newPC.get() != nullptr) modifiedComponents.push_back(newPC);
            else modifiedComponents.push_back(c);
        }

        _makeEntity(mapping.at(se.ID), modifiedComponents, se.pos);
        out.push_back(mapping.at(se.ID));
    }

    return out;
}

SystemBase* WorldBase::getSystemIndirect(SystemType t) {
    if (!typeToSystem.count(t)) return nullptr;
    return &typeToSystem.at(t);
}
