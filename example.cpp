#include "worldbase.h"
#include <iostream>

///for now, this just demonstrates how these are constructed
/// todo: make something like a simple text-based pacman game to better demonstrate

struct HealthValue {
    double maxHealth;
    double curHealth;

    HealthValue(double maxHP)
        : maxHealth(maxHP), curHealth(maxHP) {}
};

typedef TypedPartialComponent<HealthValue, SystemType::Health> HealthPC;

class ExampleGameWorld;

class HealthSystem : public SimpleSystem<HealthValue, SystemType::Health> {
    public:
    HealthSystem(std::function<Entity*(EntityID)> idToEntity, ExampleGameWorld& gw)
     : SimpleSystem(idToEntity), world(gw) {}

    void customUpdate();

    const HealthValue& get(EntityID eID) {
        assert(modules.count(eID));
        return *modules.at(eID);
    }

    void applyDamage(EntityID eID, double damage) {
        modules.at(eID)->curHealth -= damage;
    }

    private:
    ExampleGameWorld& world;
};

class ExampleGameWorld : public WorldBase {
    public:
    ExampleGameWorld();

    void customUpdate(double deltaTime);

    HealthSystem healthSystem;
	//...
};

#define ID2ENT getIDToEntityFunc()

ExampleGameWorld::ExampleGameWorld()
    : WorldBase({healthSystem}),
      healthSystem(ID2ENT, *this) /*, xSystem(ID2ENT, arg0, arg1), ...*/ {}

void ExampleGameWorld::customUpdate(double deltaTime) {
    healthSystem.update();

	//xSystem.update(arg0, arg1, ...)
	//ySystem.update(a, b, ...)
	//...
}

void HealthSystem::customUpdate() {
	auto updateFunc = [&] (EntityID eID, Entity& e, HealthValue& v) {
    	if (v.curHealth <= 0.0 + 0.00001) world.deleteEntityNextFrame(eID);
	};

	applyFunctionToModules(updateFunc);
}



/*
	=== using the system ===
*/



std::vector<std::shared_ptr<IPartialComponent>> makeComponentGroup(/*...*/) {
	return {/*XPartialComponent, YPartialComponent, ...*/};
}

int main() {
	ExampleGameWorld world;

	std::vector<std::shared_ptr<IPartialComponent>> makeComponentGroup();

	EntityID e0 = world.makeEntity(Placement(Vec3(1,0,0)), makeComponentGroup(), new HealthPC(HealthValue(15.)));

	world.update(1.0);

	std::cout<<world.getEntity(e0).pos<<std::endl;

	world.healthSystem.destroyEntityModules(e0);

	return 0;
}