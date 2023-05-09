#include "actor.h"
#include "component.h"

Entity::Entity(EntityID _ID, Placement p, const std::vector<std::reference_wrapper<IPartialComponent>>& componentList,
               std::function<SystemBase&(SystemType)> typeToSystem)
    :   ID(_ID), pos(p), prevPos(p), prevDeltaTime(1.) {
    for (auto& c : componentList) {
        relatedSystems.insert(&(c.get())(ID, typeToSystem));
    }
}


Entity::Entity(EntityID _ID, Placement p, const std::vector<std::shared_ptr<IPartialComponent>>& componentList,
               std::function<SystemBase&(SystemType)> typeToSystem)
    :   ID(_ID), pos(p), prevPos(p), prevDeltaTime(1.) {
    for (auto& c : componentList) {
        relatedSystems.insert(&(*c)(ID, typeToSystem));
    }
}


Entity::~Entity() {
    for (auto& i : relatedSystems) i->destroyEntityModules(this->ID);
}

