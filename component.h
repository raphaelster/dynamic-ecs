#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <cassert>

#include <iostream>

///maybe put this in its own file
struct EntityID {
    int ID;
    EntityID(int i)
        : ID(i) {}

    bool operator==(const EntityID& other) const { return ID == other.ID; }

    bool operator< (const EntityID& other) const { return ID < other.ID; }

    friend std::ostream& operator<<(std::ostream& o, const EntityID& e) {
        return o<<e.ID;
    }
};

template<> struct std::hash<EntityID> {
    std::size_t operator()(const EntityID& e) const {
        return e.ID;
    }
};

enum SystemType {
    Health
    //Collision,
	//X
	//Y
	//...
};

struct Entity;

struct EmptyStruct {};

///module:
/// the data for a single instance of a component
/// not a very good name, but much more concise than ComponentData or w/e so I'm going with it


///this exists so I have a base I can dynamic_cast into templated ISystems
/// also, a convenient place to put destroyEntityModules(eID), the function called to clean up after an entity being destroyed
class SystemBase {
    public:
    virtual ~SystemBase() {}

    virtual void destroyEntityModules(EntityID eID) =0;
    virtual void postCreate(EntityID eID) {}
};

///this interface exists so components don't need to know about the update input templating of a System

template <class Template>
class ISystem : public SystemBase {
    public:
    virtual ~ISystem() {}

    //virtual Instance* instantiateTemplate(const Template& t) const =0;
    //virtual void addModule(int ID, std::weak_ptr<Instance> module) =0;
    virtual void createModule(EntityID eID, const Template& t, SystemType st) =0;
};
template<class Template>
class PartialComponent;

template <class Template, class Instance, class ...UpdateInputs>
class System : public ISystem<Template> {
    std::unordered_map<EntityID, SystemType> moduleTypes;
    std::unordered_map<Instance*, EntityID> moduleToEID;
    protected:
    const std::function<Entity*(EntityID)> getEntity;

    std::unordered_map<EntityID, std::vector<std::unique_ptr<Instance>>> modules;


    virtual std::unique_ptr<Instance> instantiateTemplate(const Template& t) =0;

    virtual std::shared_ptr<PartialComponent<Template>> _recreatePartialComponent(const Instance& i, SystemType st) const =0;

    virtual void customUpdate(UpdateInputs... ui) =0;

    public:
    System(std::function<Entity*(EntityID)> _idToEntity)
        : getEntity(_idToEntity) {}

    virtual ~System() {}

    void update(UpdateInputs... ui) {
        customUpdate(ui...);
    }

    void createModule(EntityID eID, const Template& t, SystemType st) {
        modules[eID].push_back(std::move(instantiateTemplate(t)));
        moduleTypes[eID] = st;

        Instance* m = modules[eID].at(modules[eID].size()-1).get();
        moduleToEID.insert(std::pair<Instance*, EntityID>(m, eID));
    }

    void destroyEntityModules(EntityID eID) {
        preDestroy(eID);

        if (modules.count(eID)) {
            for (auto& ptr : modules.at(eID)) {
                moduleToEID.erase(ptr.get());
            }
        }

        modules.erase(eID);
        moduleTypes.erase(eID);
    }


    virtual void preDestroy(EntityID eID) {}

    EntityID moduleEID(Instance* ptr) {
        return moduleToEID.at(ptr);
    }

    std::shared_ptr<PartialComponent<Template>> recreatePartialComponent(const Instance& i, EntityID eID) const {
        return _recreatePartialComponent(i, moduleTypes.at(eID));
    }

    bool has(EntityID eID) const {
        if (modules.count(eID) == 0) return false;
        return modules.at(eID).size();
    }

    void applyFunctionToModules(std::function<void(EntityID, Entity&, Instance&)> func) {
        for (auto& i : modules) for (auto& j : i.second) {
            Entity* ePtr = getEntity(i.first);
            assert(ePtr != nullptr);
            Entity& e = *ePtr;

            func(i.first, e, *j);
        }
    }
};

struct IComponent {
    virtual ~IComponent() {};
};

struct IPartialComponent {
    virtual ~IPartialComponent() {};

    virtual SystemBase& operator()(EntityID ID, std::function<SystemBase&(SystemType)>& typeToSystem) const =0;
};

//pass these in Entity ctors to ensure Components are always coupled with valid IDs
template<class Template>
class PartialComponent : public IPartialComponent {
    Template t;
    SystemType sysType;

    public:
    PartialComponent(Template _t, SystemType st)
        : t(_t), sysType(st) {}
    virtual ~PartialComponent() {}

    SystemBase& operator()(EntityID eID, std::function<SystemBase&(SystemType)>& typeToSystem) const {
        SystemBase& rawSys = typeToSystem(sysType);
        ISystem<Template>* castedSys = dynamic_cast<ISystem<Template>*>(&rawSys);

        if (castedSys == nullptr) {
            std::string errorMsg = std::string("Failed to cast SystemBase to proper derived templated type System<");
            errorMsg += std::string(typeid(Template).name());
            errorMsg += std::string(">");
            errorMsg += std::string("\n(from System type ")+std::string(typeid(rawSys).name())+std::string(")");

            throw std::invalid_argument(errorMsg);
        }

        castedSys->createModule(eID, t, sysType);
        return *castedSys;
    }
};

class EmptyPC : public PartialComponent<EmptyStruct> {
    public:
    EmptyPC(SystemType s)
        : PartialComponent<EmptyStruct>(EmptyStruct(), s) {}
};


template <class Instance, class ...UpdateInputs>
class SimpleSystem : public System<Instance, Instance, UpdateInputs...> {
    public:
    SimpleSystem(std::function<Entity*(EntityID)> idToEntity)
     : System<Instance, Instance, UpdateInputs...>(idToEntity) {}
    virtual ~SimpleSystem() {}

    std::unique_ptr<Instance> instantiateTemplate(const Instance& i) {
        return std::make_unique<Instance>(i);
    }
    //std::shared_ptr<PartialComponent<Template>> _recreatePartialComponent(const Instance& i, SystemType st) const
    std::shared_ptr<PartialComponent<Instance>> _recreatePartialComponent(const Instance& i, SystemType st) const {
        return std::make_shared<PartialComponent<Instance>>(i, st);
    }
};

template <class ...UpdateInputs>
class TagSystem : public System<EmptyStruct, EmptyStruct, UpdateInputs...> {
    public:
    TagSystem(std::function<Entity*(EntityID)> idToEntity)
     : System<EmptyStruct, EmptyStruct>(idToEntity) {}

    std::unique_ptr<EmptyStruct> instantiateTemplate(const EmptyStruct& es) {
        return std::make_unique<EmptyStruct>();
    }
    std::shared_ptr<PartialComponent<EmptyStruct>> _recreatePartialComponent(const EmptyStruct& es, SystemType st) const {
        return std::make_shared<PartialComponent<EmptyStruct>>(EmptyStruct(), st);
    }
};

#endif // COMPONENT_H
