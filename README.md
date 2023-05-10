# dynamic-ecs
Wedge: a dynamic entity-component system, with a focus on runtime flexibility

Naturally, Wedge is an acronym, meaning:

<pre>
    W  
(fl)Exible  
    Dynamic  
    G  
    Entity-component system  
</pre>

# Features:

Create entities with a list of arbitrary components at runtime  
Serialize and deserialize Entities and Worlds (in the future)  
Automagic destruction; deleting an Entity will delete any data in any related systems  
Support for transfering Entities between different Worlds, with differing Systems

# Usage:

example.cpp has a basic example, but the general method is:  
1. Create some number of systems, which inherit from System<Template, Instance, Inputs...>
  a. Overload any virtual functions, like customUpdate, for user-specific behavior
  b. Register these systems in the SystemType enum in component.h (this is annoying, but other methods have worse drawbacks*)
2. Create a world which inherits from WorldBase
  a. Give it access to the systems created in 1.
  b. Override customUpdate to call mySystem.update(inputs) for all systems the world manages
3. Call MyWorld.makeEntity(...) for any Entities you want to create
  a. Entities can be constructed out of any 

# Terminology:
A Template is used to instantiate a component Instance; an instantiated Instance is called a module.  
A PartialComponent is a Component that isn't coupled to a specific System; this allows users to transfer Entities between different Worlds
which have different implementations of the same System (or System mocking). For example, world A might be a designer, and world B a simulator.
Systems in the designer world can be easy to mutate runtime, and systems in the simulator world can be constant and heavily optimized.  

# Misc:
*Drawbacks of other System registration methods:
  - Unique IDs could be generated compile-time with __COUNTER__, but this would corrupt saved Worlds when new Systems are added, or 
when compilation order changes. This would also not support System mocking.
  - Systems could be registered by a user-specified string or integer, but this would change typos from a compile-time error to a runtime error.
Since these typos would make Entity creation worse (the most common use case), while the current solution makes System creation worse 
(infrequent), the tradeoff is not worth it. Also, accidental ID collision is easier.
