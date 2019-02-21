# SKELETAL GAME MILESTONE PROGRESS - Feb. 1

  

Overview: For this milestone, we were able to take on more than we had originally planned to in the development plan. Below, are the details.

  

## Milestone Requirements
  

- Working application rendering code and shaders for background and sprite assets 
    - Fighters (players + AI) are loaded upon initial startup 

- Loading and rendering of textured geometry with correct blending. 
    - Potato textures onto fighters 

- Working basic 2D transformations.  
    - Basic movement (translation), fighter flips onto side when dead (rotation) 

- Keyboard/mouse control of one or more character sprites. This can include changes in the set of rendered objects, object geometry, position, orientation, textures, colors, and other attributes. 
    - Keyboard control of two characters through two different key mappings (player 1 = w,a,s,d,e; player 2 = i,j,k,l,o).  Included regular left and right movement, crouch, jump, and punch. 
    - The jump control took longer to implement than planned, as there was some trouble drawing along the y-axis, but this was eventually done. 

- Random or hard-coded action of (other) characters/assets. 
    - 1st variant of AI randomly moves around the screen 

- Basic key-frame/state interpolation (smooth movement from point A to point B in Cartesian or angle space). 
- Stable game code supporting continuing execution and graceful termination 
- Preliminary work on health system (ability to die) 
    - Expected: Depletes player health when position is out of bounds 
    - Actual: We currently have it so that player 1’s health is depleted when holding the “E” key, and player 2’s health is depleted using the “O” key 
        - Players are currently attacking themselves as a stub 

  

Creative: You should implement one or more additional creative elements. These can include additional integrated assets or rendering effects or pre-emptive implementation of one or more features from subsequent milestones.

- Planned Work 
    - Temporary health display (HP left) 
        - Displays in the window title 

- Unplanned Work 
    - Lives system: Fighters (players + ai) will revive a pre-determined amount of times before being unable to play anymore 
    - 2nd player 
    - 2nd variant of AI with decision tree (follow player) 
    - Music added on loop 
    - Creating art assets for potato and broccoli characters 
        - Sprite sheet for broccoli animation 

    - Fighter shaders 
        - Fighter is coloured red when taking damage 

    - Environment shaders 
        - The entire screen has a wavy distort effect when holding the “ENTER” key
        


# MINIMAL PLAYABILITY MILESTONE PROGRESS - Feb. 22



Overview: For this milestone, we focused on Collision System and UI (health display, menu), also fixing and adding new assets and features



## Milestone Requirements


- Sustain progressive, non-repetitive gameplay using all required features for 2 min or more (assume that you can provide users with oral instruction).
	- Collision System: allow for damaging opponents
    
- You should implement state and decision tree driven (possibly randomized) response to user input and game state (create a decision tree data structure and reuse it for multiple entities)
	- 
    
- Provide extended sprite and background assets set as well as corresponding actions.
	- stage background (kitchen)
	- new Broccoli sprite (currently used for AI)
    
- Provide basic user tutorial/help.
	- basic tutorial Mode: display controls for player 1/2 as text (FreeType)

- Stable game code supporting continuing execution and graceful termination
	- Fixed reset button: allow for continuous replay without closing the game
    


Creative: You should implement one or more additional creative elements. These can include additional integrated assets or rendering effects or pre-emptive implementation of one or more features from subsequent milestones.
	
- Planned Work
	- Improved Health display: FreeType text rendering in layers
	- Title menu: allow players to choose between one-player/two-player/tutorial modes
 		- random AI in menu to display basic art and gameplay


