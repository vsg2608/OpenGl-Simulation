# OpenGl-Simulation
Simulation of Downhill Skiing - Hierarchical Modeling and Animation

  ->The modeling of a skier needs to be represented as a hierarchical model with an articulated structure. The model can be seen as a tree.
  
  ->The primitives to model a skier should be from OpenGL primitves.
  
  ->The models should be texture mapped.
  
  ->The hierarchical model defines a heirarchy where a transformation when applied to a node in the tree its sub tree inherits the transformation.
  
  ->The animation module will involve defining some key frames of the skier, which may form a particular type of motion for the purpose of skiing when interpolated. The parameter of interpolation can be correlated with time.
  
  ->You can change the parameter of interpolation to perform variation in the way the motion takes place. This can be achieved by using appropriate function.
  
  ->You need to also model accessories -- the skies, the poles, the inclined plane. The skies may be anchored at the feet and the poles may be acnhored at hands.
  
  ->The path on the plane can be defined through a spline curve.
  
  ->The camera can follow the skiier, can be at fixed locations on looking at a zone of the whole scene, and a wide angle camera enabling the view of the entire scene.
  
  ->At some locations a flag with a pole can be installed. This flag when is hit by the skier may fall.

Playground.cpp is the file that contains all the basic code for opengl
Common folder contains other opengl cpp files that are used in project

To Simulate
Download the code and import the project in codeblocks(Assignment2.cbp file can be used to import things) and run playground.cpp
