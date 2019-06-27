Flocking Plugin

SETUP:

Create a new Blueprint Class From Boid -> this will be your boid, full weights and ranges are settable through the blueprint. You can either add it a mesh
or a particle system in order to have a biggest swarm without having problems due to draw calls.


TARGETS:
If you want your flock to move around a pattern or towards a target you can add elements to its Targets Actors Array. If none is added the Flok will roam around freely.

You can keep your flock at a certain distance from the target setting it's range.

OBSTACLES SETUP:

Create a Blueprint class inheriting from AvoidablesManager. -> this will be the actor that will keep count of the spawned obstacles.
Create a Blueprint class inheriting from Avoidable. -> this will be your obstacle.

Place the avoidableManager in the scene.

In the new Avoidable BP class set it's AvoidableManager AM reference with the new AvoidableManager class you put in the scene.

If the Obstacle you are creating will be destroyed remember to remove it from AM->Avoidables array before destroying it.

In Your Manager class create a custom event that will Get All Actors of the obstacle class you created and assign the out array to the internal Avoidables Array:
Bind this custom event to the OnChangeAvoidablesNum Delegate.

Assign Your Manager to the boids AM variable.

HINT: if you are using particle systems on your boids thjink about using Attractors with negative strength for a more realistic effect.