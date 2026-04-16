# Scene.setOnMouseClicked Event Handling
## Complete Technical Deep Dive from Your Warrior Game

---

## Table of Contents
1. [Overview](#overview)
2. [What is setOnMouseClicked?](#what-is-setOnmouseclicked)
3. [Event Handling Basics](#event-handling-basics)
4. [Lambda Expressions & Event Handlers](#lambda-expressions--event-handlers)
5. [MouseEvent Object](#mouseevent-object)
6. [Your Implementation Explained](#your-implementation-explained)
7. [Hit Detection (Bounds.contains)](#hit-detection-boundscontains)
8. [Backward Iteration Logic](#backward-iteration-logic)
9. [Complete Code Flow](#complete-code-flow)
10. [Advanced Event Handling](#advanced-event-handling)
11. [Debugging & Testing](#debugging--testing)

---

## Overview

### The Problem

Your game needs to respond when a user clicks on the screen:
- **Detect**: Where did the user click?
- **Identify**: Which warrior was clicked?
- **React**: Select or deselect that warrior

**Solution**: Use `scene.setOnMouseClicked()` to register an event handler.

### The Core Concept

```
User clicks on screen at (120, 100)
         ↓
JavaFX detects the click
         ↓
Creates a MouseEvent object with click data
         ↓
Calls the registered event handler
         ↓
Your code executes with the event data
```

---

## What is setOnMouseClicked?

### Method Signature

```java
scene.setOnMouseClicked(EventHandler<MouseEvent> handler);
```

**Breaking it down:**
- `scene` = the display surface where clicks happen
- `setOnMouseClicked()` = register a handler for clicks
- `EventHandler<MouseEvent>` = code to run when clicked
- `handler` = the actual event handling code

### Simple Example

```java
// Register a handler
scene.setOnMouseClicked(event -> {
    System.out.println("User clicked!");
    System.out.println("X: " + event.getX());
    System.out.println("Y: " + event.getY());
});
```

### In Your Project

```java
// From HelloApplication.java
scene.setOnMouseClicked(event -> {
    for (int i = warriors.size() - 1; i >= 0; i--) {
        Unit warrior = warriors.get(i);
        if (warrior.tryActivate(event.getX(), event.getY())) {
            break;
        }
    }
});
```

---

## Event Handling Basics

### Traditional Event Handling (Anonymous Class)

Before lambda expressions, you had to create an inner class:

```java
scene.setOnMouseClicked(new EventHandler<MouseEvent>() {
    @Override
    public void handle(MouseEvent event) {
        System.out.println("Clicked!");
        System.out.println("X: " + event.getX());
    }
});
```

**Verbose and repetitive!**

### Modern Event Handling (Lambda)

Java 8+ allows shorter lambda syntax:

```java
scene.setOnMouseClicked(event -> {
    System.out.println("Clicked!");
    System.out.println("X: " + event.getX());
});
```

**Much cleaner!**

### How They're Equivalent

```java
// Lambda version
event -> { ... }

// Is short for:
new EventHandler<MouseEvent>() {
    @Override
    public void handle(MouseEvent event) {
        ...
    }
}
```

The lambda automatically:
- Creates the anonymous class
- Implements `handle()` method
- Takes the `event` parameter

---

## Lambda Expressions & Event Handlers

### Lambda Syntax Explained

```java
(parameters) -> { body }
  ↑            ↑  ↑
  Parameters   Arrow  Code block
```

### In Your Code

```java
scene.setOnMouseClicked(event -> {
    //   ↑
    //   Single parameter (MouseEvent)
    //   Type is inferred from context
    
    // Body: code to execute
    for (int i = warriors.size() - 1; i >= 0; i--) {
        // ...
    }
});
```

### Different Lambda Styles

```java
// Single statement - no braces needed
scene.setOnMouseClicked(event -> System.out.println("Clicked"));

// Multiple statements - braces required
scene.setOnMouseClicked(event -> {
    System.out.println("Clicked at: " + event.getX() + ", " + event.getY());
    System.out.println("Button: " + event.getButton());
});

// No parameters (other event types)
button.setOnAction(event -> System.out.println("Button pressed"));

// Multiple parameters (if applicable)
// callback.execute((a, b) -> a + b);
```

### What is EventHandler<MouseEvent>?

```java
// EventHandler is an interface
public interface EventHandler<T extends Event> {
    void handle(T event);  // Method to implement
}

// For MouseEvent specifically:
public interface EventHandler<MouseEvent> {
    void handle(MouseEvent event);  // Called when mouse event happens
}

// Your lambda implements this interface
event -> { ... }
// Becomes:
void handle(MouseEvent event) { ... }
```

---

## MouseEvent Object

### What is MouseEvent?

`MouseEvent` is an object containing **all data about a mouse click**:

```java
public class MouseEvent extends InputEvent {
    double x;           // X coordinate of click
    double y;           // Y coordinate of click
    MouseButton button; // Which button (PRIMARY, SECONDARY, MIDDLE)
    int clickCount;     // Single click, double click, etc.
    // ... and more properties
}
```

### Key Properties in Your Game

| Property | Code | Example | Meaning |
|----------|------|---------|---------|
| X coordinate | `event.getX()` | `120.5` | Pixels from left edge |
| Y coordinate | `event.getY()` | `100.3` | Pixels from top edge |
| Button | `event.getButton()` | `MouseButton.PRIMARY` | Which mouse button |
| Click count | `event.getClickCount()` | `1` or `2` | Single or double click |

### Getting Click Position

```java
// Your code uses:
event.getX()  // Returns the X coordinate as double
event.getY()  // Returns the Y coordinate as double

// Used in:
if (warrior.tryActivate(event.getX(), event.getY())) {
    //                     ↑            ↑
    //                     X            Y
}

// Example:
// User clicks at position (120, 100) on screen
// event.getX() returns 120.0
// event.getY() returns 100.0
```

### Other Useful Properties

```java
// Check which button was clicked
if (event.getButton() == MouseButton.PRIMARY) {
    // Left mouse button
}

// Check for double-click
if (event.getClickCount() == 2) {
    // Double click detected
}

// Check for modifiers (Shift, Ctrl, Alt)
if (event.isControlDown()) {
    // Ctrl key held while clicking
}

// Get the scene where click happened
Scene source = (Scene) event.getSource();
```

---

## Your Implementation Explained

### Full Code Breakdown

```java
scene.setOnMouseClicked(event -> {
    // Loop through warriors BACKWARDS (from last to first)
    for (int i = warriors.size() - 1; i >= 0; i--) {
        Unit warrior = warriors.get(i);
        
        // Try to activate this warrior
        if (warrior.tryActivate(event.getX(), event.getY())) {
            break;  // Exit loop if successful
        }
    }
});
```

### Line-by-Line Analysis

#### Line 1: Register the Handler

```java
scene.setOnMouseClicked(event -> {
//     ↑
//     When the scene detects a mouse click,
//     execute the following code
```

**What happens:**
1. User clicks anywhere on the 1080×700 scene
2. JavaFX creates a `MouseEvent` with click data
3. Passes it to your lambda: `event -> { ... }`

#### Line 2: Loop Setup

```java
for (int i = warriors.size() - 1; i >= 0; i--) {
//     ↑                          ↑        ↑
//     Start at last warrior      Condition: while ≥ 0
//                                Decrement (go backwards)
```

**Example with 3 warriors:**
```
warriors.size() = 3
warriors indices: [0, 1, 2]

Loop iterations:
i = 2 (last warrior)
i = 1 (middle warrior)
i = 0 (first warrior)
i = -1 (exit loop)
```

**Why backward iteration?** See section [Backward Iteration Logic](#backward-iteration-logic)

#### Line 3: Get Warrior

```java
Unit warrior = warriors.get(i);
//   ↑                      ↑
//   Store reference    Get warrior at index i
```

**Example:**
```java
warriors.get(2);  // Gets the 3rd warrior (last added)
warriors.get(1);  // Gets the 2nd warrior
warriors.get(0);  // Gets the 1st warrior
```

#### Line 4-6: Try to Activate

```java
if (warrior.tryActivate(event.getX(), event.getY())) {
    //       ↑                          ↑
    //       Call method               Pass X, Y coordinates
    break;
    // Exit loop if hit detected
}
```

**What `tryActivate()` does:**
```java
// From Unit.java
public boolean tryActivate(double mx, double my) {
    if (image.getBoundsInParent().contains(mx, my)) {
        //                            ↑  ↑
        //                            Check if (mx, my) is inside image
        flipActivation();  // Toggle selection
        return true;       // Signal: hit detected
    }
    return false;          // Signal: no hit
}
```

**Logic:**
```
if (hit detected)
    → Toggle selection
    → Return true
    → Loop breaks (exit)
else
    → Return false
    → Loop continues (check next warrior)
```

---

## Hit Detection (Bounds.contains)

### What is getBoundsInParent()?

`Bounds` is a rectangle representing the visual area occupied by a node:

```java
image.getBoundsInParent();
// Returns: Bounds object

// Bounds contains:
// minX = left edge
// minY = top edge
// maxX = right edge
// maxY = bottom edge
// width = maxX - minX
// height = maxY - minY
```

### Example: Bounds Calculation

```java
// Image at (80, 80) with size 100×100

Bounds bounds = image.getBoundsInParent();
//
// bounds.minX = 80
// bounds.minY = 80
// bounds.maxX = 180  (80 + 100)
// bounds.maxY = 180  (80 + 100)
// bounds.width = 100
// bounds.height = 100

// Visual:
// ┌─────────────────┐
// │ (80,80)         │
// │                 │
// │  100×100        │
// │                 │
// │         (180,180)
// └─────────────────┘
```

### How contains() Works

```java
bounds.contains(mx, my)
//     ↑
//     Returns true if point (mx, my) is inside bounds rectangle

// Implementation (conceptual):
if (mx >= bounds.minX && mx <= bounds.maxX &&
    my >= bounds.minY && my <= bounds.maxY) {
    return true;  // Inside
}
return false;     // Outside
```

### Example: Click Detection

```
Warrior image at (80, 80), size 100×100
Bounds: minX=80, minY=80, maxX=180, maxY=180

User Click #1: (120, 100)
- Is 120 >= 80? YES
- Is 120 <= 180? YES
- Is 100 >= 80? YES
- Is 100 <= 180? YES
→ contains() returns TRUE → Hit detected!

User Click #2: (200, 200)
- Is 200 >= 80? YES
- Is 200 <= 180? NO
→ contains() returns FALSE → No hit
```

### Visual Representation

```
Scene 1080×700

     80              180
     │              │
 80  ┌──────────────┐
     │ [Image]      │
     │ bounds =     │
     │ (80,80) to   │  ← Hit zone (green)
180  │ (180,180)    │
     └──────────────┘

Clicks inside green area: detected
Clicks outside green area: not detected

Example positions:
✓ (120, 100)  - inside
✓ (90, 90)    - inside
✓ (170, 170)  - inside
✗ (60, 80)    - outside (too left)
✗ (200, 100)  - outside (too right)
✗ (120, 200)  - outside (too low)
```

---

## Backward Iteration Logic

### The Problem: Click Priority

You have 3 warriors on screen, with overlapping positions:

```
Warrior #1: (80, 80)
Warrior #2: (100, 100)  ← Overlaps with #1
Warrior #3: (120, 120)  ← Overlaps with #1 and #2

Visual:
     ┌──────────────────┐
     │ Warrior #3       │
     │ ┌────────────────┤
     │ │ Warrior #2     │
     │ │ ┌──────────────┤
     │ │ │ Warrior #1   │
```

### User clicks at (110, 110)

This point is inside bounds of ALL 3 warriors!

**Question**: Which warrior should be selected?

**Answer**: The one **on top** (visually last drawn)

### Forward Iteration (❌ WRONG)

```java
for (int i = 0; i < warriors.size(); i++) {
    // i = 0: Warrior #1 → click is inside → selected (❌ WRONG!)
    // Stops here, never checks #2 or #3
}

// Result: User clicks on #3, but #1 is selected
// Very frustrating UI!
```

### Backward Iteration (✅ CORRECT)

```java
for (int i = warriors.size() - 1; i >= 0; i--) {
    // i = 2: Warrior #3 → click is inside → selected (✅ CORRECT!)
    // Stops here
}

// Result: User clicks on #3, and #3 is selected
// Intuitive UI!
```

### Why It Works

JavaFX renders nodes **in the order they're added to the Group**:

```java
group.getChildren().addAll(labelName, life, image);

// First added → rendered first (bottom)
// Last added  → rendered last (top, visible on top)
```

In your loop:
1. Start checking warriors from the **end** of the list (last added)
2. These warriors are **visually on top**
3. So they're checked first
4. If user clicks on overlapping warriors, the **topmost one** is selected
5. `break` stops the loop immediately

---

## Complete Code Flow

### Scenario: Click at (120, 100) on 3 Warriors

**Initial Setup:**
```
Warrior #0 at (80, 80)   - bottom
Warrior #1 at (260, 120) - middle  
Warrior #2 at (460, 220) - top

Added to group in order 0, 1, 2
Rendering: #0 first (bottom), then #1, then #2 (top)
```

**Visual:**
```
┌─────────────────────────────────────────┐
│                                         │
│  [#0]    [#1]    [#2]                  │
│         ▲
│      Click here (120, 100)
│      → Only #0's bounds contain this point
│
└─────────────────────────────────────────┘
```

### Step-by-Step Execution

#### Step 1: User Clicks

```
User clicks at screen position (120, 100)
```

#### Step 2: Event Generated

```java
// JavaFX detects click and creates:
MouseEvent event = new MouseEvent(...);
event.x = 120.0;
event.y = 100.0;
event.button = MouseButton.PRIMARY;
event.clickCount = 1;
```

#### Step 3: Handler Called

```java
scene.setOnMouseClicked(event -> {
    // Handler is invoked with event
    // event.getX() returns 120.0
    // event.getY() returns 100.0
    
    for (int i = warriors.size() - 1; i >= 0; i--) {
        // i starts at 2 (last index)
```

#### Step 4: Check Warrior #2

```java
// i = 2
Unit warrior = warriors.get(2);  // Warrior at (460, 220)

// Check bounds
Bounds bounds = image.getBoundsInParent();
// bounds: (460, 220) to (560, 320)

boolean hit = bounds.contains(120, 100);
// Is 120 in [460, 560]? NO
// → contains() returns false
// → No hit detected

// Continue loop (no break)
```

#### Step 5: Check Warrior #1

```java
// i = 1
Unit warrior = warriors.get(1);  // Warrior at (260, 120)

// Check bounds
Bounds bounds = image.getBoundsInParent();
// bounds: (260, 120) to (360, 220)

boolean hit = bounds.contains(120, 100);
// Is 120 in [260, 360]? NO
// → contains() returns false
// → No hit detected

// Continue loop (no break)
```

#### Step 6: Check Warrior #0

```java
// i = 0
Unit warrior = warriors.get(0);  // Warrior at (80, 80)

// Check bounds
Bounds bounds = image.getBoundsInParent();
// bounds: (80, 80) to (180, 180)

boolean hit = bounds.contains(120, 100);
// Is 120 in [80, 180]? YES ✓
// Is 100 in [80, 180]? YES ✓
// → contains() returns true
// → Hit detected!

if (warrior.tryActivate(event.getX(), event.getY())) {
    // tryActivate() was called
    // warrior.isActive toggled
    // green selection box added/removed
    
    break;  // Exit loop immediately
}
```

#### Step 7: Visual Update

```java
// After tryActivate() returns true:

// Green selection border appears around Warrior #0
// User sees: "I clicked on #0, and #0 was selected"
// Success!
```

---

## Advanced Event Handling

### Technique 1: Detect Which Button

```java
scene.setOnMouseClicked(event -> {
    if (event.getButton() == MouseButton.PRIMARY) {
        // Left click
        selectWarrior(event.getX(), event.getY());
    } else if (event.getButton() == MouseButton.SECONDARY) {
        // Right click
        attackWarrior(event.getX(), event.getY());
    }
});
```

### Technique 2: Double-Click Detection

```java
scene.setOnMouseClicked(event -> {
    if (event.getClickCount() == 2) {
        // Double click
        moveWarriorToPosition(event.getX(), event.getY());
    } else {
        // Single click
        selectWarrior(event.getX(), event.getY());
    }
});
```

### Technique 3: Keyboard Modifier Detection

```java
scene.setOnMouseClicked(event -> {
    if (event.isControlDown()) {
        // Ctrl + click
        addWarriorToSelection(event.getX(), event.getY());
    } else if (event.isShiftDown()) {
        // Shift + click
        selectWarriorRange(event.getX(), event.getY());
    } else {
        // Regular click
        selectWarrior(event.getX(), event.getY());
    }
});
```

### Technique 4: Multiple Event Types

```java
// Click
scene.setOnMouseClicked(event -> {
    System.out.println("Clicked at: " + event.getX() + ", " + event.getY());
});

// Move
scene.setOnMouseMoved(event -> {
    System.out.println("Moving over: " + event.getX() + ", " + event.getY());
});

// Drag
scene.setOnMouseDragged(event -> {
    System.out.println("Dragging at: " + event.getX() + ", " + event.getY());
});

// Release
scene.setOnMouseReleased(event -> {
    System.out.println("Released at: " + event.getX() + ", " + event.getY());
});
```

### Technique 5: Consuming Events

```java
// Prevent other handlers from processing this event
scene.setOnMouseClicked(event -> {
    selectWarrior(event.getX(), event.getY());
    
    event.consume();  // Stop event propagation
});
```

---

## Debugging & Testing

### Debug: Print Click Information

```java
scene.setOnMouseClicked(event -> {
    // Print all click data
    System.out.println("=== CLICK DEBUG ===");
    System.out.println("X: " + event.getX());
    System.out.println("Y: " + event.getY());
    System.out.println("Button: " + event.getButton());
    System.out.println("Click Count: " + event.getClickCount());
    System.out.println("Ctrl Down: " + event.isControlDown());
    System.out.println("Shift Down: " + event.isShiftDown());
    
    for (int i = warriors.size() - 1; i >= 0; i--) {
        Unit warrior = warriors.get(i);
        Bounds bounds = warrior.image.getBoundsInParent();
        boolean hit = bounds.contains(event.getX(), event.getY());
        
        System.out.println("Warrior " + i + " hit: " + hit);
        System.out.println("  Bounds: " + bounds);
    }
});
```

### Debug: Highlight Click Zone

```java
// Add visual indicator of where click registered
private Circle clickIndicator = new Circle(5);

scene.setOnMouseClicked(event -> {
    clickIndicator.setCenterX(event.getX());
    clickIndicator.setCenterY(event.getY());
    clickIndicator.setFill(Color.RED);
    
    if (!group.getChildren().contains(clickIndicator)) {
        group.getChildren().add(clickIndicator);
    }
    
    // Rest of logic...
});
```

### Test: Unit Tests for Hit Detection

```java
@Test
public void testHitDetection() {
    Warrior warrior = new Warrior(100, true, "ally", 5, false, 
                                  new ArrayList<>(), 80, 80);
    
    // Simulate click at (120, 100)
    boolean hit = warrior.image.getBoundsInParent().contains(120, 100);
    assertTrue(hit);
    
    // Simulate click at (200, 200)
    boolean miss = warrior.image.getBoundsInParent().contains(200, 200);
    assertFalse(miss);
}
```

### Common Issues & Fixes

| Issue | Cause | Fix |
|-------|-------|-----|
| Clicks don't register | Handler not registered | Call `scene.setOnMouseClicked()` |
| Wrong warrior selected | Forward iteration | Use backward loop: `i--` |
| Bounds always null | Image not added to scene | Ensure `resurrect()` called |
| Click position wrong | Coordinates misunderstood | Print `event.getX()`, `event.getY()` |
| Double-click not detected | Wrong click count check | Use `event.getClickCount() == 2` |

---

## Summary: Key Takeaways

| Concept | What It Does | Your Code |
|---------|-------------|-----------|
| `setOnMouseClicked()` | Registers click handler | `scene.setOnMouseClicked(event -> { ... })` |
| `MouseEvent` | Contains click data | `event.getX()`, `event.getY()` |
| Lambda expression | Shorthand for event handler | `event -> { ... }` |
| `getBoundsInParent()` | Gets visual area of node | `image.getBoundsInParent().contains()` |
| `contains()` | Checks if point in bounds | Hit detection |
| Backward iteration | Check topmost first | `for (i = size-1; i >= 0; i--)` |
| `break` | Exit loop immediately | Stop checking after hit found |

---

## Event Handling Flowchart

```
┌─ User clicks on screen
│
├─ JavaFX creates MouseEvent
│   ├─ x = click X
│   ├─ y = click Y
│   └─ button, clickCount, modifiers...
│
├─ Calls scene.setOnMouseClicked(event)
│
├─ Your lambda executes
│   for (i = size-1; i >= 0; i--)
│     warrior = warriors.get(i)
│     if (warrior.tryActivate(x, y))
│       return true  → Hit!
│     break
│
├─ tryActivate() checks bounds.contains(x, y)
│   ├─ YES → flipActivation() → return true
│   └─ NO → return false
│
└─ Visual result: selection box appears/disappears
```

---

**Created**: April 16, 2026  
**Project**: OOP LABA#4 - Warrior Game with JavaFX  
**Focus**: Event Handling Deep Dive
