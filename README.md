
# PLAYABILITY MILESTONE PROGRESS - Mar. 8



Overview: For this milestone, we focused on improving the gameplay by introducing character selection, new abilities, also fixing and adding many new assets and features



## Milestone Requirements


- Provide complete playable prior-milestone implementation
	- everything from previous milestones are kept and improved upon

- Sustain progressive, non-repetitive gameplay for 4min or more including all new features (with minimal tutorial) 
	- Character Selection System: allow player to choose between 2 veggie characters
    - New fighting abilities
 		- Blocking (blocking enemy attacks, but cannot initiate attacks)
 		- Bullet (linear projectile)
		- Projectile (parabolic projectile)
 		- Power punching (charging up a punch that deals more damage)
	- platform + enviornmental death (falling)

- Implement time stepping based physical animation: A subset of the game entities (main or background) should now possess non-trivial physics properties such as linear momentum or angular momentum, and acceleration and 
  act based on those. Specifically, you should implement some form of physical simulation, which can be either background effects (e.g. water, smoke implemented using particles) or active game elements (throwing a ball,
  swinging a rope, etc…)
	- gravity-based acceleration
	- physical simulation for fighter moves
 		- projectile (parabolic projectile using gravity)
        
- Incorporate one or more polygonal geometric assets 
	- creating mesh on Blender (chef hat), still in progress
- Implement smooth non-linear motion of one or more assets or characters 
	- jumping for all characters are gravity-based (force)
    
- Implement an accurate and efficient collision detection method (include multiple moving assets that necessitate collision checks)
	- fighters/projectiles/bullets/platforms all require collision checks
    
- Stable game code supporting continuing execution and graceful termination.
	- polished paused menu
 		- fixed: pausing all background actions
		- resume: supporting continuing execution
		- going back to main menu: allow players to switch modes without closing the game
		- quit: graceful termination
        - restart: resets game after game over

Creative: You should implement one or more additional creative elements. These can include additional integrated assets or rendering effects or pre-emptive implementation of one or more features from subsequent milestones.
	
- Planned Work
	- new animations:
		- Potato holding power punches + release
		- replaces all crouches with actual texture instead of changing position
		- Broccoli punches, crouching
	- fighting moves:
    	- Bullet (linear projectile)
		- Projectile (parabolic projectile)
	- platform + enviornmental death (falling)
	- new background music
    
- Unplanned Work
	- polished paused menu (resume, main menu, quit)
	- power punching
    - Blocking: effects using shaders
	- improving main menu
	- fixed grunting sounds
    - cooldown to punches, special abilities, and blocking
    - heatwave stage effect (cycles every 19 seconds currently)
    	- does damage to players onscreen, but can be blocked


# MINIMAL PLAYABILITY MILESTONE PROGRESS - Feb. 22



Overview: For this milestone, we focused on the Collision System and UI (health display, menu) as suggested by the TA feedback from the last milestone, also fixing and adding many new assets and features



## Milestone Requirements


- Sustain progressive, non-repetitive gameplay using all required features for 2 min or more (assume that you can provide users with oral instruction).
	- Collision System: allow for hitting and damaging opponents
	- Mode System: one-player/two-player/tutorial modes (and switching between)
    
- You should implement state and decision tree driven (possibly randomized) response to user input and game state (create a decision tree data structure and reuse it for multiple entities)
	- Smarter state and decision tree driven AI
		- e.g. AI punches when player is punching
    
- Provide extended sprite and background assets set as well as corresponding actions.
	- Extended Sprite assets
    	- new Idle and punching animation (for Potato)
    	- new Broccoli sprite (currently used for AI)
	- Background for "Kitchen" stage 
	
    
- Provide basic user tutorial/help.
	- Basic tutorial Mode: display controls for player and UI as text (FreeType)
 		- also display controls when game is paused (ESC key)

- Stable game code supporting continuing execution and graceful termination
	- Fixed reset button: allow for continuous replay without closing the game
    


Creative: You should implement one or more additional creative elements. These can include additional integrated assets or rendering effects or pre-emptive implementation of one or more features from subsequent milestones.
	
- Planned Work
	- Improved Health display: FreeType text rendering in layers (Health Points and Stock)
	- Main title menu: allow players to choose between modes (one-player/two-player/tutorial) wth temporary background image
 		- random AI in menu to display basic art and gameplay

- Unplanned Work
	- Pausing and resuming gameplay(ESC)
	- Music Controls (PageDown/PageUp for Volume, End/Home for pause/play)
	- Player punching-grunt sound effects when punch key is released
	- Nameplates above all fighters in a battle


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
     

