# JavaFX Basic Components Guide
## Using Examples from Warrior Game Project

---

## Table of Contents
1. [Overview](#overview)
2. [Stage](#stage)
3. [Scene](#scene)
4. [Group](#group)
5. [Nodes (Visual Elements)](#nodes-visual-elements)
   - [ImageView](#imageview)
   - [Label](#label)
   - [Line](#line)
   - [Rectangle](#rectangle)
6. [Event Handling](#event-handling)
7. [Coordinate System](#coordinate-system)
8. [Component Relationships](#component-relationships)

---

## Overview

JavaFX follows a **hierarchical structure** for displaying graphics:

```
Stage (Window)
  └─ Scene (Display Surface)
      └─ Group (Container)
          └─ Nodes (Visual Elements: Images, Text, Shapes)
```

Your Warrior game uses all these components to create an interactive battlefield.

---

## Stage

### What is a Stage?

A **Stage** is the top-level JavaFX container - it's the **window** that displays on your screen.

### In Your Project

```java
// From HelloApplication.java - this is implicit
@Override
public void start(Stage stage) throws IOException {
    group = new Group();
    scene = new Scene(group, 1080, 700);
    
    stage.setScene(scene);      // Add scene to stage
    stage.show();               // Display the window
}
```

### Key Properties

| Property | Example | Purpose |
|----------|---------|---------|
| `setScene()` | `stage.setScene(scene)` | Connect a Scene to the Stage |
| `setTitle()` | `stage.setTitle("Warrior Battle")` | Window title bar text |
| `setWidth()` | `stage.setWidth(1080)` | Window width in pixels |
| `setHeight()` | `stage.setHeight(700)` | Window height in pixels |
| `show()` | `stage.show()` | Make window visible |

### Analogy

Think of **Stage** as a picture frame. The frame itself (Stage) holds the picture (Scene).

---

## Scene

### What is a Scene?

A **Scene** is the **display surface** where all graphics are drawn. It has dimensions (width × height) and contains a **root node** (usually a Group).

### In Your Project

```java
group = new Group();
scene = new Scene(group, 1080, 700);
//                  ↑     ↑     ↑
//              root node │    height
//                      width
```

**What this means:**
- **Root node**: `group` contains all warriors, labels, health bars
- **Width**: 1080 pixels
- **Height**: 700 pixels

### Key Properties

| Property | Example | Purpose |
|----------|---------|---------|
| Constructor | `new Scene(root, 1080, 700)` | Create scene with root and size |
| `getRoot()` | `scene.getRoot()` | Get the root node |
| `getWidth()` | `scene.getWidth()` | Get scene width |
| `setOnMouseClicked()` | See [Event Handling](#event-handling) | Register click handler |

### Why Scene Matters

The Scene is responsible for:
- **Rendering**: Drawing all nodes efficiently
- **Event propagation**: Passing mouse/keyboard events to nodes
- **Layout**: Positioning nodes on the display

---

## Group

### What is a Group?

A **Group** is a **container node** that holds other nodes. It's like a folder that keeps visual elements organized.

### In Your Project

```java
group = new Group();
scene = new Scene(group, 1080, 700);

// Later: add warriors to group
for (Unit warrior : warriors) {
    warrior.resurrect();  // Calls group.getChildren().addAll(...)
}
```

### Key Operations

| Operation | Code | Purpose |
|-----------|------|---------|
| Add single node | `group.getChildren().add(labelName)` | Make node visible |
| Add multiple nodes | `group.getChildren().addAll(label, line, image)` | Batch add |
| Remove node | `group.getChildren().remove(rectActive)` | Make node invisible |
| Get all children | `group.getChildren()` | Access all nodes |
| Clear all | `group.getChildren().clear()` | Remove everything |

### In Your Warrior Class

```java
public void resurrect() {
    if (HelloApplication.group == null) return;
    
    // Add: label, health bar, image
    HelloApplication.group.getChildren().addAll(labelName, life, image);
    
    // Add selection box only if active
    if (isActive) {
        HelloApplication.group.getChildren().add(rectActive);
    }
}

public void flipActivation() {
    if (HelloApplication.group != null) {
        if (isActive) {
            HelloApplication.group.getChildren().remove(rectActive);
        } else {
            HelloApplication.group.getChildren().add(rectActive);
        }
    }
    isActive = !isActive;
}
```

**Key insight**: Adding/removing from `group.getChildren()` controls visibility!

### Hierarchy in Your Project

```
Group (HelloApplication.group)
├── Warrior #1
│   ├── Label "Warrior"
│   ├── Line (health bar)
│   ├── ImageView (warrior image)
│   └── Rectangle (selection border) [only if active]
├── Warrior #2
│   ├── Label "Warrior"
│   ├── Line (health bar)
│   ├── ImageView (warrior image)
│   └── Rectangle (selection border) [only if active]
└── Warrior #3
    ├── Label "Warrior"
    ├── Line (health bar)
    ├── ImageView (warrior image)
    └── Rectangle (selection border) [only if active]
```

---

## Nodes (Visual Elements)

Nodes are the **actual visual components** - they're what you see on screen.

### ImageView

#### What is ImageView?

An **ImageView** displays an **image** (PNG, JPG, etc.) on the screen.

#### In Your Project

```java
// HelloApplication.java - Load image once
URL warriorUrl = HelloApplication.class.getResource(
    "/warrior_backup_before_transparency-removebg-preview.png"
);
imgWarrior = new Image(warriorUrl.toExternalForm(), 100, 100, false, false);
//                                                    ↑    ↑   ↑    ↑
//                                                  width height preserve smooth


// Warrior.java - Create ImageView for each warrior
image = new ImageView(HelloApplication.imgWarrior);
image.setX(startX);
image.setY(startY);
```

#### Constructor Parameters Explained

```java
new Image(url, width, height, preserveRatio, smooth)
```

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `url` | `"warrior_...png"` | Image file path |
| `width` | `100` | Display width in pixels |
| `height` | `100` | Display height in pixels |
| `preserveRatio` | `false` | Don't maintain original aspect ratio |
| `smooth` | `false` | Don't apply smoothing (faster) |

#### Key Properties

| Property | Code | Purpose |
|----------|------|---------|
| Set position | `image.setX(80); image.setY(80)` | Place at (80, 80) |
| Get bounds | `image.getBoundsInParent()` | Rectangle around image |
| Set opacity | `image.setOpacity(0.8)` | Transparency (0.0 = invisible, 1.0 = visible) |
| Rotate | `image.setRotate(45)` | Rotate 45 degrees |
| Scale | `image.setScaleX(1.5); image.setScaleY(1.5)` | Enlarge by 150% |

#### In Your setCoordinates Method

```java
public void setCoordinates() {
    // ... other code ...
    
    image.setX(x + imageDeltaX());      // Position from x, y
    image.setY(y + imageDeltaY());
    
    // imageDeltaX/Y allow fine-tuning without changing x, y
}
```

**Note:** In the current project, imageDeltaX/Y return 0.0, so the image positions at (x, y) directly. However, the delta pattern allows subclasses to override and add offsets if needed.

---

### Label

#### What is Label?

A **Label** displays **text** on the screen.

#### In Your Project

```java
// Warrior.java - Create label with warrior name
this.labelName = new Label("Warrior");
labelName.setLayoutX(x);
labelName.setLayoutY(y);
```

#### Key Properties

| Property | Code | Purpose |
|----------|------|---------|
| Set text | `label.setText("Warrior")` | Change displayed text |
| Get text | `label.getText()` | Read current text |
| Set position | `label.setLayoutX(100); label.setLayoutY(50)` | Place at (100, 50) |
| Set font | `label.setFont(Font.font(14))` | Font size 14 |
| Set color | `label.setTextFill(Color.WHITE)` | Text color |
| Set style | `label.setStyle("-fx-text-fill: red;")` | CSS styling |

#### In Your setCoordinates Method

```java
public void setCoordinates() {
    labelName.setLayoutX(x);    // Always above image
    labelName.setLayoutY(y);
    
    // ... other updates ...
}
```

---

### Line

#### What is Line?

A **Line** is a shape that draws a **straight line** between two points.

#### In Your Project

```java
// Warrior.java - Create health bar as a line
life = new Line();
life.setStrokeWidth(5);                    // Line thickness
life.setStroke(Color.LIGHTGREEN);          // Line color

// In setCoordinates - update position dynamically
double hp = getHealth() == null ? 0.0 : Math.max(0.0, getHealth());

life.setStartX(x);                         // Start at warrior's X
life.setStartY(y + 15);                    // Slightly below

life.setEndX(x + (hp / MAX_HEALTH) * 100); // Extend based on health %
life.setEndY(y + 15);                      // Same Y as start
```

#### How Health Bar Works

```
100 health:  ▓▓▓▓▓▓▓▓▓▓ (100 pixels long)
 75 health:  ▓▓▓▓▓▓▓▓░░ (75 pixels long)
 50 health:  ▓▓▓▓▓░░░░░ (50 pixels long)
 25 health:  ▓▓░░░░░░░░ (25 pixels long)
  0 health:  ░░░░░░░░░░ (0 pixels - invisible)
```

**Formula**: `endX = startX + (currentHealth / maxHealth) * barWidth`

#### Key Properties

| Property | Code | Purpose |
|----------|------|---------|
| Start point | `line.setStartX(x); line.setStartY(y)` | Beginning of line |
| End point | `line.setEndX(x2); line.setEndY(y2)` | End of line |
| Thickness | `line.setStrokeWidth(5)` | Line thickness in pixels |
| Color | `line.setStroke(Color.RED)` | Line color |
| Opacity | `line.setOpacity(0.7)` | Transparency |

---

### Rectangle

#### What is Rectangle?

A **Rectangle** is a shape that draws a **rectangular box**.

#### In Your Project

```java
// Warrior.java - Create selection border
rectActive = new Rectangle(x - 5, y - 5, 110, 110);
//                         ↑     ↑   ↑    ↑
//                      startX startY width height

rectActive.setFill(Color.TRANSPARENT);     // No fill (hollow)
rectActive.setStroke(Color.GREEN);         // Border color
rectActive.setStrokeWidth(3);              // Border thickness
```

#### In Your setCoordinates Method

```java
public void setCoordinates() {
    // ... other updates ...
    
    rectActive.setX(x - 5);     // Offset by -5 to center around image
    rectActive.setY(y - 5);
}
```

#### Visual Representation

```
Image:              Rectangle (Selection):
┌─────────────┐     ┌──────────────────┐
│  [100×100]  │     │ [110×110]        │
└─────────────┘     └──────────────────┘

x=100, y=100        x=95, y=95 (offset by -5)
```

#### Key Properties

| Property | Code | Purpose |
|----------|------|---------|
| Position | `rect.setX(50); rect.setY(50)` | Top-left corner at (50, 50) |
| Size | `rect.setWidth(100); rect.setHeight(100)` | 100×100 dimensions |
| Fill | `rect.setFill(Color.BLUE)` | Interior color (TRANSPARENT = hollow) |
| Stroke | `rect.setStroke(Color.RED)` | Border color |
| Stroke width | `rect.setStrokeWidth(3)` | Border thickness |

#### Constructor vs Setters

```java
// Constructor (all parameters at once)
Rectangle rect1 = new Rectangle(x, y, width, height);

// Setters (individually)
Rectangle rect2 = new Rectangle();
rect2.setX(x);
rect2.setY(y);
rect2.setWidth(width);
rect2.setHeight(height);
```

---

## Event Handling

### What are Events?

**Events** are things that happen:
- User clicks mouse
- Presses keyboard
- Resizes window
- etc.

### Mouse Click Event in Your Project

```java
// HelloApplication.java
scene.setOnMouseClicked(event -> {
    // event.getX() = mouse X coordinate
    // event.getY() = mouse Y coordinate
    
    // Check each warrior from back to front
    for (int i = warriors.size() - 1; i >= 0; i--) {
        Unit warrior = warriors.get(i);
        if (warrior.tryActivate(event.getX(), event.getY())) {
            break;  // Stop after first hit - exit immediately
        }
    }
});
```

**For a detailed technical explanation of `scene.setOnMouseClicked()`, including:**
- Lambda expressions and event handlers
- MouseEvent object properties
- Hit detection with `Bounds.contains()`
- Backward iteration logic (why it matters)
- Complete execution flow with examples
- Advanced event handling techniques
- Debugging and testing strategies

**See the dedicated guide**: [JAVAFX_MOUSECLICKED_EVENT.md](JAVAFX_MOUSECLICKED_EVENT.md)

### Hit Detection in Unit.java

```java
public boolean tryActivate(double mx, double my) {
    // Get the rectangular area occupied by the image
    Bounds bounds = image.getBoundsInParent();
    
    // Check: is the click inside this rectangle?
    if (bounds.contains(mx, my)) {
        flipActivation();  // Toggle selection
        return true;       // Found a hit
    }
    return false;          // Missed
}
```

### How Bounds Work

```
Image at (80, 80) with 100×100 size:

        60      80              180
        │       │               │
     60 ┌───────┼───────────────┐
        │       │               │
        │   MISS│  HIT ZONE     │
     80 │───────┼───────────────│
        │       │ [100×100 img] │
        │       │               │
    180 └───────────────────────┘
        
Click at (120, 100): INSIDE bounds → Hit detected
Click at (60, 80):   OUTSIDE bounds → No hit
```

### Event Properties

| Property | Code | Example |
|----------|------|---------|
| Mouse X | `event.getX()` | `120.5` |
| Mouse Y | `event.getY()` | `100.3` |
| Button pressed | `event.getButton()` | `MouseButton.PRIMARY` |
| Click count | `event.getClickCount()` | `1` (single), `2` (double) |

### Other Event Types

```java
// Keyboard
scene.setOnKeyPressed(event -> {
    System.out.println("Key: " + event.getCode());
});

// Mouse movement
scene.setOnMouseMoved(event -> {
    System.out.println("Mouse: " + event.getX() + ", " + event.getY());
});

// Mouse drag
scene.setOnMouseDragged(event -> {
    System.out.println("Dragging...");
});
```

---

## Coordinate System

### Origin (0, 0)

In JavaFX, **coordinates start at the top-left**:

```
(0, 0) ─────────────────► X increases →
  │
  │
  ▼
Y increases ↓

Example: (100, 50) = 100 pixels right, 50 pixels down from top-left
```

### In Your Game

```
Scene: 1080 × 700

(0, 0)                          (1080, 0)
  ┌─────────────────────────────┐
  │                             │
  │  Warrior #1 at (80, 80)     │
  │  [img]                      │
  │                             │
  │      Warrior #2 at (260,120)│
  │      [img]                  │
  │                             │
  │           Warrior #3        │
  │           at (460, 220)     │
  │           [img]             │
  │                             │
  │                             │
  │                             │
  └─────────────────────────────┘
(0, 700)                        (1080, 700)
```

### Coordinate Update in setCoordinates()

```java
public void setCoordinates() {
    labelName.setLayoutX(x);        // Use current x, y
    labelName.setLayoutY(y);
    
    // ... health bar updates ...
    
    image.setX(x + imageDeltaX());
    image.setY(y + imageDeltaY());
    
    rectActive.setX(x - 5);         // Offset for centering
    rectActive.setY(y - 5);
}
```

---

## Component Relationships

### Full Component Hierarchy

```
┌─────────────────────────────────────────────────────┐
│                     Stage                           │ ← Window
│  ┌───────────────────────────────────────────────┐  │
│  │              Scene (1080×700)                 │  │ ← Display surface
│  │  ┌─────────────────────────────────────────┐  │  │
│  │  │          Group                          │  │  │ ← Container
│  │  │  ┌────────────────────────────────────┐ │  │  │
│  │  │  │  Warrior #1 (at 80, 80)           │ │  │  │
│  │  │  │  ├─ Label "Warrior"               │ │  │  │
│  │  │  │  ├─ Line (health bar)             │ │  │  │ ← Visual nodes
│  │  │  │  ├─ ImageView (100×100)           │ │  │  │
│  │  │  │  └─ Rectangle (selection)         │ │  │  │
│  │  │  └────────────────────────────────────┘ │  │  │
│  │  │  ┌────────────────────────────────────┐ │  │  │
│  │  │  │  Warrior #2 (at 260, 120)         │ │  │  │
│  │  │  │  ├─ Label "Warrior"               │ │  │  │
│  │  │  │  ├─ Line (health bar)             │ │  │  │
│  │  │  │  ├─ ImageView (100×100)           │ │  │  │
│  │  │  │  └─ Rectangle (selection)         │ │  │  │
│  │  │  └────────────────────────────────────┘ │  │  │
│  │  │  ┌────────────────────────────────────┐ │  │  │
│  │  │  │  Warrior #3 (at 460, 220)         │ │  │  │
│  │  │  │  ├─ Label "Warrior"               │ │  │  │
│  │  │  │  ├─ Line (health bar)             │ │  │  │
│  │  │  │  ├─ ImageView (100×100)           │ │  │  │
│  │  │  │  └─ Rectangle (selection)         │ │  │  │
│  │  │  └────────────────────────────────────┘ │  │  │
│  │  └─────────────────────────────────────────┘ │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### Data Flow When Health Changes

```
Unit.setHealth(50)
  ↓
Unit.setCoordinates()  [Updates all visual positions]
  ├─ life.setEndX(...)  [Health bar gets shorter]
  ├─ image.setX(...)    [Image repositioned]
  ├─ labelName.setLayoutX(...)  [Label repositioned]
  └─ rectActive.setX(...)  [Selection box repositioned]
  ↓
Scene automatically re-renders
  ↓
User sees updated health bar
```

### Data Flow When User Clicks

```
User clicks at (120, 100)
  ↓
scene.setOnMouseClicked(event)  [Event handler fires]
  ↓
Loop through warriors (backwards)
  ├─ Warrior #3 at (460, 220): 120,100 outside bounds → skip
  ├─ Warrior #2 at (260, 120): 120,100 outside bounds → skip
  └─ Warrior #1 at (80, 80): 120,100 INSIDE bounds! → tryActivate()
  ↓
warrior.tryActivate(120, 100)
  ├─ image.getBoundsInParent().contains(120, 100) → TRUE
  └─ flipActivation()
    ├─ isActive = true
    └─ group.getChildren().add(rectActive)  [Add green border]
  ↓
Scene re-renders
  ↓
User sees green selection box appear
```

---

## Quick Reference Table

| Component | Purpose | Added To | Example |
|-----------|---------|----------|---------|
| **Stage** | Window | (automatic) | `stage.show()` |
| **Scene** | Display surface | Stage | `new Scene(group, 1080, 700)` |
| **Group** | Container | Scene | `new Group()` |
| **ImageView** | Display image | Group | `new ImageView(imgWarrior)` |
| **Label** | Display text | Group | `new Label("Warrior")` |
| **Line** | Health bar | Group | `life.setStroke(Color.LIGHTGREEN)` |
| **Rectangle** | Selection box | Group | `new Rectangle(x-5, y-5, 110, 110)` |

---

## Summary

**JavaFX Components in Your Project:**

1. **Stage** → The window (created by framework)
2. **Scene** → The display area (1080×700)
3. **Group** → Container for all warriors
4. **ImageView** → Warrior image (reused from `imgWarrior`)
5. **Label** → "Warrior" text name
6. **Line** → Green health bar
7. **Rectangle** → Green selection border
8. **Event Handlers** → Mouse clicks to select warriors

**Key Principle**: Logic (Unit class) is separate from visuals (nodes). Update x/y in Unit, call `setCoordinates()` to update all visual nodes automatically.

---

## Next Steps to Extend Your Project

```java
// Example: Add damage numbers when attacking
private Label damageLabel;

// In Warrior.initGraphics()
damageLabel = new Label("0");
damageLabel.setTextFill(Color.RED);

// In Unit.attack()
target.damageLabel.setText(String.valueOf(this.damage));

// Animate fade-out
PauseTransition delay = new PauseTransition(Duration.seconds(1));
delay.play();
```

---

**Created**: April 16, 2026  
**Project**: OOP LABA#4 - Warrior Game with JavaFX  
**File**: `JAVAFX_COMPONENTS_GUIDE.md`
