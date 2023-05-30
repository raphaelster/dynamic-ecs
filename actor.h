#ifndef ACTOR_H
#define ACTOR_H

#include <queue>
#include <stack>

#include <unordered_set>

#include "component.h"

#include "3dmath.h"

struct GameWorld;

struct ActorInfo {
    double walkSpeed;
    double jumpHeight;

    ActorInfo(double walk, double jump)
        : walkSpeed(walk), jumpHeight(jump) {}
};

struct SavedEntity {
    EntityID ID;
    std::vector<std::shared_ptr<IPartialComponent>> components;
    Placement pos;

    SavedEntity(EntityID eid, std::vector<std::shared_ptr<IPartialComponent>>&& componentList, Placement p)
        : ID(eid), components(componentList), pos(p) {}
};

class Entity {
    EntityID ID;
    std::unordered_set<SystemBase*> relatedSystems;
    Placement prevPos;
    double prevDeltaTime;

    public:
    Placement pos;

    Entity(EntityID _ID, Placement p, const std::vector<std::reference_wrapper<IPartialComponent>>& componentList,
           std::function<SystemBase&(SystemType)> typeToSystem);
    Entity(EntityID _ID, Placement p, const std::vector<std::shared_ptr<IPartialComponent>>& componentList,
           std::function<SystemBase&(SystemType)> typeToSystem);
    ~Entity();

    Entity(const Entity&) = delete;
    Entity& operator= (const Entity&) = delete;

    EntityID getID() const { return ID; }

    void updatePrevPos(double deltaTime) {
        prevPos = pos;
        prevDeltaTime = deltaTime;
    }

    Placement getPrevPos() const {
        return prevPos;
    }

    Vec3 getFrameVel() const {
        return pos.pos - prevPos.pos;
    }

    Vec3 getVel() const {
        return getFrameVel() / prevDeltaTime;
    }

    Quaternion getFrameOmega() const {
        return pos.dir * prevPos.dir.conjugate();
    }

    const std::unordered_set<SystemBase*>& getRelatedSystems() { return relatedSystems; }

	//called in the WorldBase dtor; systems are deleted before entities, so this avoids a crash when worlds are destroyed
	void clearRelatedSystems() {
		relatedSystems.clear();
	}

	//needed so connectSystem can keep track of the lifetime of a limb tree parent
	void addRelatedSystem(SystemBase* b) {
        relatedSystems.insert(b);
	}

	SavedEntity save();

};

#endif // ACTOR_H
