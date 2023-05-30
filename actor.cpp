#include "actor.h"
#include "worldbase.h"

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


SavedEntity Entity::save() {
    std::vector<std::shared_ptr<IPartialComponent>> outList;

    for (SystemBase* s : relatedSystems) {
        std::vector<std::shared_ptr<IPartialComponent>> list = s->recreatePartialComponents(ID);

        outList.insert(outList.end(), list.begin(), list.end());
    }

    return SavedEntity(ID, std::move(outList), pos);
}
