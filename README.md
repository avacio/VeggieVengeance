# GRAND FINALE MILESTONE PROGRESS - Apr. 19



Overview: For this final milestone, we focus on improving the ai and fixing past issues regarding stage hazards, as well as adding final touches and balancing the game.



## Milestone Requirements


- Development: All features implemented in the previous milestones should be working, or improved upon if it’s the case.
	- all abilities are included, and their damages are tuned
    
- Robustness: Sustain progressive, non-repetitive gameplay across one or more levels for 10min including all new features. No verbal explanation should be required at any point during the gameplay
	- 4 fighters (9 abilities), 2-player/1-player v ai modes, 2 stages (with unique hazards) provide non-repetitive fighting matches
    
        
- Usability: Include a self-explanatory tutorial introducing the player to the game mechanics
	- tutorial mode includes all controls (player controls, music controls etc.)
    	- updated to include optional controls for player 2
        
- External Integration: Include integration of one or more external tools or libraries (physical simulation (PhysX, Bullet, ODE, etc …), game engines, or alternatives)
	- freeType library for text rendering
    
- Advanced Graphics: Implementation of one or more advanced graphics features including visual effects (Particle Systems, 2.5D(3D) lighting, 2D dynamic shadows) and/or advanced 2D geometric modifications (2D deformations, rigged/skinned motion)
	- new Particle system
		- fighters emit particle as visual effects when charging and using power punch
    - new stage (oven)
        
- Advanced Gameplay: Implementation of one or more advanced gameplay features including advanced decision making mechanisms based on goals (path planning, A*, or similar), or some form of group behavior if applicable to the game; more complex physic interactions with the environment (e.g. gravity, bouncing, complex dynamics)
	- AIs pathfind to approach player in 1-player mode
	- gravity applies to all fighters
    - bouncing implemented for projectile (Broccoli's cauliflower)
    
- Accessibility: evaluate and optimizing user-game interactions (choice of user gestures, ease of navigation, etc …)
	- added quit game option in main menu
	- pause menu (esc)
	- all player controls should be very comfortable for 2-handed play (refined according to crossplay feedback)
		- player 1 controls: wasd, cvb, left shift
		- player 2 controls: arrow keys, ",./" keys, right shift (if keyboard includes numpad: 123 + enter)

- Audio: There should be audio feedback for all meaningful interactions in the game as well as a background music with tones reflecting the current state of the game.
	- randomly selected background music out of a list of 8 
	- new sound effects for most fighter abilities (bullets, projectile, bomb, uppercut, dash, heal, emoji-shot)
    

# ROBUST MILESTONE PROGRESS - Mar. 29



Overview: For this milestone, we continued to introduce new features such as characters, abiltiies and stages, while also fixing and optimizing the performance.



## Milestone Requirements


- Provide complete playable prior-milestone implementation
	- features from previous milestones are kept and improved upon (e.g. abilities rework)
    
- Sustain progressive, non-repetitive gameplay for 6min or more including all new features. The game should not terminate early and should allow infinite even if repetitive gameplay
	- Complete menu system, allowing for non-repetitive gameplay e.g. rematches, switching characters, stage select
	- New Fighters
		- Eggplant: Circling emojis, emoji bullets
		- Yam: Self-healing, dash
	- New fighting abilties
		- Potato: Tater tot bomb, chargable bullets (fries)
		- Broccoli: Uppercut, chargable projectiles (cauliflowers)
	- balancing states
		- introduced tired-out state if players are abusing the charged abilities (fighter tired out if holding abilities for too long) 
	- more platforms (resembling battlefield in smash)

- Support real-time response rate (i.e. lag-free input).
	- gameplay are lag-free
	- optimize loading and unloading resources -> reduced loading match time to under 1 sec

- Include proper memory management (no excessive allocation or leaks). The game should not have any undefined behavior, memory leaks or random crashes. The game should not hog memory even after extended play time. 
	- resources e.g. bomb, bullets, projectiles are destroyed
	- memory usage usually plateaus at around 256MB

- The game should robustly handle any user input. Unexpected inputs or environment settings should be correctly handled and reported

- The gameplay should be real-time (no lag). This included improving your collision handling using effective detection strategies. You should support dozens simultaneously moving main or background assets
	- smooth collision detection for characters, platforms, spawned assets (bomb, bullets, projectiles)

- The game should allow for some form of state saving for play “reload”. Users should be able to pause and restart in a graceful (if not perfect) manner. 
	- matches can be paused and restart gracefully

- The physical effects should be correctly integrated in time and should not be locked to the machine’s speed by correctly handling the simulation time step and integration.

- Stable game code supporting continuing execution and graceful termination.


Creative: You should implement one or more additional creative elements. These can include additional integrated assets or rendering effects or pre-emptive implementation of one or more features from subsequent milestones.
	
- Planned Work
	- new animations:
		- Charging, tired out, death for all fighters
	- new fighter moves:
		- different projectile types
        	- bouncy cauliflower
            - circling emojis that you can fire
        - tater tot bombs
    - mesh
    	- knives
    - stage select
    - eggplant character with sprites
    - refined animations for player states
- Unplanned Work
	- stage effect of falling knives with collision and attack damage
    - add yam character with sprites!
    - fighters have a HP stat based on template character


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
    - game over screen
    	- quit, return to menu, restart
    - character select screen
    	- AI changes based on character type that is currently selected
        - shows stats
        - will determine actual player type

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
     

