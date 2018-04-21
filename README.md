# TeddyWickets

Teddy Wickets
Copyright 2017 - Matthew Carlin

------

Version history:


April 2018 - V0.2 - One Minute Demo

This is the first playable version of the game, from title screen to win condition. Changes:

- Support for multiple modes (ie different screens)
- Title screen
- Model loader (models come from MayaLT)
- Model viewer (separate program)
- Add font support with SDL2_ttf
- Controller support
- Keymapping / controller mapping
- Game controller / keyboard setup screen
- Bear selection mode in the 2p game
- Improved lighting model
- Abstract away OpenGL in wrappers
- Upgrade from OpenGL 2.1 to OpenGL 4.1 and rewrite graphics to fit
- Cel shading
- Improved color palette
- Model caching
- Level editor (save, load, make levels)
- HotConfig
- Remove mouse support
- Music loops, multiple music tracks
- Player base colors
- Show scores
- Fix the 2d rotation problem
- Clean HUD first draft
- Fix the major graphics memory leak problem (by translate/rotate)
- Wicket crossing detection and action
- Redo physics/state transitions
- Bear to bear collisions (should not screw up with state transitions; it should not reset the bear to drop mode)
- Front Bumpers
- Ramp colors
- Start spot color
- Shot predictions
- Move the baddy
- Take out velocity lines
- Final wicket and simple celebration

------

December 2017 - V0.1 - Clean Organized Rewrite

This is a clean and organized rewrite of the V0.0 sketch, with additional changes:

- makefile
- make a larger level and (limited) camera tracking
- make the ball roll
- add a reasonable looking amount of friction
- add simple mountain underneath the level
- add bumpers
- add music to make sure there are no issues there
- clear up the color issue with png files
- get the background and a token piece of heads up display working
- make the 3d bear a little cuter
- add lighting
- get orthographic perspective working
- add a reset mechanic for when the bear stops moving
- make the bumpers light up when bumped
- take mouse input with SDL
- add a shot mechanic for keyboard
- add prediction arrows to show the direction of the shot based on a forward bullet simulation
- rename textures and refactor texture loading
- make physics updates use time instead of framerate
- display control information
- add a shot mechanic for mouse 
- add some obstacles to hit or avoid with the ball
- lint the code for style (using cpplint)
- submit code review (thanks, Avi Robinson-Mosher)

------

October 2017 - V0.0 -  First sketch.

Testing four things:

- SDL framework
- 2d sprite ball on either a 2d world or a 3d ortho world
- Keyboard input to move the ball
- Bullet Physics engine

------

Future version wishlist:

- add cel shading
- get several options for the ball sprite