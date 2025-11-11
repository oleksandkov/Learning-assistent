# Advanced Climbing Game

A realistic climbing game where you control a character climbing a wall with advanced hand-switching and body-swinging mechanics.

## Project Structure

```
the game/
 include/
    ClimbingGame.h      # Game class header
 src/
    ClimbingGame.cpp    # Game implementation
    main.cpp            # Entry point
 bin/
    main.exe            # Compiled executable
 Fonts/                  # Font directory
```

## Advanced Features

###  Dual Control System
- **Active Hand Mode**: Control the currently active (free) hand
- **Automatic Switching**: When you grab with one hand, control switches to the other
- **Body Swing Control**: Use the inactive hand controls to swing your body weight!

###  Realistic Physics
- **Arm Length Limit**: Hands can only reach 60 pixels from shoulders
- **Weight Dynamics**: Body hangs realistically from grabbed hands
- **Gravity System**: Falls when both hands are free
- **Smooth Interpolation**: Natural movement and swinging

###  Game Elements
- **Height Tracking**: Real-time height display in top-left corner
- **Camera Tracking**: Follows character vertically as you climb
- **5000px Wall**: Tall climbing wall with grip indicators
- **Visual Feedback**: Color changes show which hand is grabbed

## Controls

### Starting State
- Game starts with **LEFT HAND** active
- Use **WASD** to move the left hand
- Press **SPACE** to grab with left hand
- Control automatically switches to right hand

### Control Flow

#### When Controlling LEFT Hand (Red)
- **W/A/S/D**: Move left hand up/left/down/right
- **SPACE**: Grab with left hand  switches to right hand control
- **Hand can only move when NOT grabbing**

#### When Controlling RIGHT Hand (Blue)  
- **Arrow Keys**: Move right hand up/down/left/right
- **SPACE**: Grab with right hand  switches to left hand control
- **Hand can only move when NOT grabbing**

###  Body Swinging Mechanics

This is the advanced feature you requested!

#### When Right Hand is Active (Controlling Right Hand)
- **Left hand is grabbed** (holding your weight)
- Use **A/D** (left hand controls) to swing body left/right
- Creates momentum for dynamic climbing

#### When Left Hand is Active (Controlling Left Hand)
- **Right hand is grabbed** (holding your weight)
- Use **Left/Right Arrows** (right hand controls) to swing body left/right
- Helps reach farther grip points

**Swing Range**: 30 pixels from center  
**Swing Decay**: Automatically dampens over time (realistic physics)

## How to Play

### Basic Climbing Sequence

1. **Start**: Left hand active, move it up the wall (WASD)
2. **Grab**: Press SPACE when positioned on wall
3. **Switch**: Control automatically switches to right hand
4. **Move**: Use Arrow keys to move right hand higher
5. **Grab**: Press SPACE to grab with right hand
6. **Switch**: Control switches back to left hand
7. **Release**: Press SPACE again to release the lower hand
8. **Repeat**: Keep alternating to climb higher!

### Advanced Techniques

#### Dynamic Swinging
1. Grab with one hand
2. While controlling the other hand, use the grabbed hand's keys to swing
3. Swing creates horizontal momentum
4. Reach grip points that would be too far normally
5. Grab at the peak of your swing for maximum distance

#### Strategy Tips
- **Plan ahead**: Look for grip points before releasing hands
- **Use momentum**: Swing to reach distant grips
- **Stay balanced**: Too much swing makes it harder to grab accurately
- **Watch the colors**: Darker hand = grabbed, bright = active control

## Compilation

```powershell
# Navigate to project
cd "c:\Users\muaro\Documents\GitHub\Learning-assistent\c++ sfml\the game"

# Compile
g++ src/main.cpp src/ClimbingGame.cpp -Iinclude -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -o bin/main.exe -lsfml-graphics -lsfml-window -lsfml-system

# Run
./bin/main.exe
```

## Game Mechanics Explained

### Hand Control State Machine
```
LEFT Active  SPACE (grab)  RIGHT Active  SPACE (grab)  LEFT Active
                                 
  WASD moves hand            Arrows move hand
  Arrows swing body          WASD swings body
```

### Physics System

#### Body Position Calculation
- **Both hands grabbed**: Body hangs between them + swing offset
- **Left hand only**: Body hangs from left hand + swing offset  
- **Right hand only**: Body hangs from right hand + swing offset
- **No hands**: Falls with gravity

#### Arm Constraints
```cpp
maxArmLength = 60 pixels
distance = sqrt((handX - shoulderX) + (handY - shoulderY))
if (distance > maxArmLength)  movement blocked
```

## Visual Elements

- **Red Hand**: Left hand (WASD control)
- **Blue Hand**: Right hand (Arrow control)
- **Dark Red/Blue**: Hand is grabbing
- **Green Body**: Torso
- **Beige Head**: Character head
- **Brown Feet**: Automatically positioned
- **White Lines**: Arms and legs connecting to body
- **Gray Wall**: Climbing surface with grip indicators

## UI Display

### Top-Left Corner
```
Height: XX.X m
```

### Bottom
```
Controlling: LEFT (WASD) | Space: Grab | Other keys: Swing body
```
or
```
Controlling: RIGHT (Arrows) | Space: Grab | Other keys: Swing body
```

## Technical Details

- **Frame Rate**: 60 FPS
- **Window Size**: 800x600 pixels
- **Wall Height**: 5000 pixels (~83 screen heights)
- **Gravity**: 400 units/s
- **Swing Speed**: 100 units/s
- **Interpolation**: 15% per frame for smooth hanging
- **Swing Decay**: 95% per frame

## Troubleshooting

**Font not loading?**
- Game tries Arial first, then Calibri
- Will run without fonts but no text display

**Hands won't move?**
- Check if hand is grabbed (darker color)
- Grabbed hands cannot move - press SPACE to release

**Can't reach a grip?**
- Try swinging your body to extend reach
- Make sure you're within 60-pixel arm length

**Character falling?**
- At least one hand must be grabbed to stay up
- Both hands free = gravity pulls you down

## Code Architecture

### Class Structure
- **ClimbingGame**: Main game class
  - Manages window, view, game loop
  - Handles all input and physics
  - Renders all game elements

### Key Methods
- `handleInput()`: Processes WASD/Arrow keys + swing controls
- `toggleGrab()`: Grabs/releases and switches active hand
- `updatePhysics()`: Calculates body position and applies gravity
- `updateCamera()`: Follows character vertically
- `render()`: Draws everything in correct order

### State Management
- `activeHand`: Tracks which hand is currently controllable
- `bodySwingOffset`: Stores horizontal body displacement
- `isGrabbing`: Per-hand boolean for grab state

## Enjoy Climbing!

Try to reach the top of the 5000-pixel wall. Master the swinging mechanics to make dynamic climbing moves!
