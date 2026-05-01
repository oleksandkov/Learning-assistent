package com.example.laba_main;

import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Objects;
import java.util.Scanner;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Line;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;




public class Unit implements Cloneable{
    private Integer health;
    private Boolean isSpawned;
    protected  boolean team;
    private Integer damage;
    private Boolean isDead;
    private ArrayList<String> inventor;

    private static int numObjects = 0;
    private static int objectedKilled = 0;

    // Graphical arguments
    protected Label labelName;
    protected Line life;
    protected ImageView image;
    protected double x, y;
    protected boolean isActive;
    protected Rectangle rectActive;
    protected double maxHealth;
    protected ImageView imageMarkRed;
    protected ImageView imageMarkGreen;
    protected ImageView imageMark;
    protected ImageView swordImage;
    protected ImageView knifeImage;
    protected ImageView spearImage;
    protected ImageView bowImage;
    protected ImageView mainWeaponImage;

    // Movement and attack timing
    protected double moveSpeed = 0.005;


    // Attack and logic variables
    protected long lastAttackTime = 0; 
    protected  final long ATTACK_COOLDOWN = 1000;

    URL swordUrl = getClass().getResource("/sword.png");
    URL knifeUrl = getClass().getResource("/knife.png");
    URL spearUrl = getClass().getResource("/spear.png");
    URL bowUrl = getClass().getResource("/bow.png");


    protected void setMaxHealth(double maxHealth) {
        this.maxHealth = maxHealth;
    }

    protected double labelDeltaX() {
        return 10.0;
    }

    protected double labelDeltaY() {
        return -10.0;
    }

    protected double lifeDeltaX() {
        return 0.0;
    }

    protected double lifeDeltaY() {
        return 10.0;
    }

    protected double imageDeltaX() {
        return 0.0;
    }

    protected double imageDeltaY() {
        return 0.0;
    }

    protected double rectDeltaX() {
        return -9.0;
    }

    protected double rectDeltaY() {
        return -9.0;
    }

    public boolean isActive() {
        return isActive;
    }

    

    static {
        System.out.println("STATIC BLOCK IS RUNT");
        System.out.println("Unit class loaded. Objects count: " + numObjects);
    }

    {
        numObjects++;
        System.out.println("INIT BLOCK IS RUNT");
        System.out.println("Object #" + numObjects + " is being initialized");
    }
    public Unit(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this.health = health;
        this.isSpawned = isSpawned;
        this.team = team;
        this.damage = damage;
        this.isDead = isDead;
        this.inventor = inventor;
        System.out.println("Constructor: Unit(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor)");
        System.out.println("Object created. Total objects: " + numObjects);
    }

    public  Unit() {
        this(100, false, true, 5, false, new ArrayList<String>(Arrays.asList("sword")));
    }

    public  boolean getActive() {
    return isActive;

    }
    public static int getNumObjects() {
        return numObjects;
    }

    public static void setNumObjects(int num) {
        numObjects = num;
    }

    public Integer getHealth() {
        return health;
    }

    public Boolean getSpawned() {
        return isSpawned;
    }

    public boolean getTeam() {
        return team;
    }

    public Integer getDamage() {
        return damage;
    }

    public Boolean getDead() {
        return isDead;
    }

    public ArrayList<String> getInventor() {
        return inventor;
    }

    public static int getnumObjects() {
        return numObjects;
    }

    public javafx.scene.image.ImageView getImage() {
        return image;
    }

    public static class HealthComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return Integer.compare(a.getHealth(), b.getHealth());
        }
    }

    public static class TeamComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return Boolean.compare(a.getTeam(), b.getTeam());
        }
    }

    public int compareTo47(Unit x) {
        int cmp = Integer.compare(this.health, x.health);
        if (cmp != 0) return cmp;
        cmp = Boolean.compare(this.team, x.team);
        if (cmp != 0) return cmp;
        cmp = this.damage.compareTo(x.damage);
        if (cmp != 0) return cmp;
        cmp = this.isSpawned.compareTo(x.isSpawned);
        if (cmp != 0) return cmp;
        cmp = this.isDead.compareTo(x.isDead);
        if (cmp != 0) return cmp;
        cmp = this.health.compareTo(x.health);
        if (cmp != 0) return cmp;
        ArrayList<String> sortedInventor1 = new ArrayList<>(this.inventor);
        ArrayList<String> sortedInventor2 = new ArrayList<>(x.inventor);
        sortedInventor1.sort(String::compareTo);
        sortedInventor2.sort(String::compareTo);
        cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
        if (cmp != 0) return cmp;
        return 0;
    }

    public static Comparator<Unit> comparatorFromTemplate(Unit template) {
        return (u1, u2) -> {
            if (template == null) {
                throw new IllegalArgumentException("Template cannot be null");
            }
            int cmp;
            if (template.getHealth() != null) {
                cmp = Integer.compare(u1.getHealth(), u2.getHealth());
                if (cmp != 0) return cmp;
            }
            if (template != null) {
                cmp = Boolean.compare(u1.getTeam(), u2.getTeam());
                if (cmp != 0) return cmp;
            }
            if (template.getDamage() != null) {
                cmp = u1.getDamage().compareTo(u2.getDamage());
                if (cmp != 0) return cmp;
            }
            if (template.getSpawned() != null) {
                cmp = u1.getSpawned().compareTo(u2.getSpawned());
                if (cmp != 0) return cmp;
            }
            if (template.getDead() != null) {
                cmp = u1.getDead().compareTo(u2.getDead());
                if (cmp != 0) return cmp;
            }
            if (template.getInventor() != null) {
                ArrayList<String> sortedInventor1 = new ArrayList<>(u1.getInventor());
                ArrayList<String> sortedInventor2 = new ArrayList<>(u2.getInventor());
                sortedInventor1.sort(String::compareTo);
                sortedInventor2.sort(String::compareTo);
                cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
                if (cmp != 0) return cmp;
            }
            return 0;
        };
    }


     protected  void setHealth(Integer health) {
        this.health = health;
        
        if (this.health != null && this.health < 0) {
            
            removeUnitFromGame();
            return;
        }
        
        if (life != null) {
            setCoordinates();
        }
    }
    
    public void removeUnitFromGame() {
        this.isDead = true;
        
        if (HelloApplication.group != null) {
            if (this.labelName != null) {
                HelloApplication.group.getChildren().remove(this.labelName);
            }
            if (this.life != null) {
                HelloApplication.group.getChildren().remove(this.life);
            }
            if (this.image != null) {
                HelloApplication.group.getChildren().remove(this.image);
            }
            if (this.rectActive != null) {
                HelloApplication.group.getChildren().remove(this.rectActive);
            }
            if (this.imageMark != null) {
                HelloApplication.group.getChildren().remove(this.imageMark);
            }
        }
        
        if (HelloApplication.units != null) {
            HelloApplication.units.remove(this);
        }
        
        removeUnit();
        System.out.println("Unit removed from game completely.");
    }

    public void setSpawned(Boolean spawned) {
        isSpawned = spawned;
    }

    public void setTeam(boolean team) {
        this.team = team;
        
    }

    public void setDamage(Integer damage) {
        this.damage = damage;
    }

    public void setDead(Boolean dead) {
        isDead = dead;
    }

    public void setInventor(ArrayList<String> inventor) {
        this.inventor = inventor;
        inventoryLogic();
    }

    

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return Objects.equals(health, unit.health) && Objects.equals(isSpawned, unit.isSpawned) && team == unit.team && Objects.equals(damage, unit.damage) && Objects.equals(isDead, unit.isDead) && Objects.equals(inventor, unit.inventor);
    }

    @Override
    public int hashCode() {
        return Objects.hash(health, isSpawned, team, damage, isDead, inventor);
    }


    @Override
    public String toString() {
        return "Unit{" +
                "health=" + health +
                ", isSpawned=" + isSpawned +
                ", Team='" + team + '\'' +
                ", damage=" + damage +
                ", isDead=" + isDead +
                ", inventor=" + inventor +
                "}\n";
    }


    @Override
    protected Unit clone() throws CloneNotSupportedException {

        Unit clonedUnit = (Unit) super.clone();
        ArrayList<String> clonedInventor = (ArrayList<String>) clonedUnit.inventor.clone();

        clonedUnit.setInventor(clonedInventor);

        if (this.labelName != null) {
            clonedUnit.labelName = new Label(this.labelName.getText());
        }

        if (this.life != null) {
            clonedUnit.life = new Line();
            clonedUnit.life.setStrokeWidth(this.life.getStrokeWidth());
            clonedUnit.life.setStroke(this.life.getStroke());
        }

        if (this.image != null) {
            clonedUnit.image = new ImageView(this.image.getImage());
            clonedUnit.image.setFitWidth(this.image.getFitWidth());
            clonedUnit.image.setFitHeight(this.image.getFitHeight());
        }

        if (this.rectActive != null) {
            double rectWidth = this.rectActive.getWidth();
            double rectHeight = this.rectActive.getHeight();
            clonedUnit.rectActive = new Rectangle(rectWidth, rectHeight);
            clonedUnit.rectActive.setFill(this.rectActive.getFill());
            clonedUnit.rectActive.setStroke(this.rectActive.getStroke());
            clonedUnit.rectActive.setStrokeWidth(this.rectActive.getStrokeWidth());
        }

        if (this.imageMarkRed != null) {
            clonedUnit.imageMarkRed = new ImageView(this.imageMarkRed.getImage());
            clonedUnit.imageMarkRed.setFitWidth(this.imageMarkRed.getFitWidth());
            clonedUnit.imageMarkRed.setFitHeight(this.imageMarkRed.getFitHeight());
        }

        if (this.imageMarkGreen != null) {
            clonedUnit.imageMarkGreen = new ImageView(this.imageMarkGreen.getImage());
            clonedUnit.imageMarkGreen.setFitWidth(this.imageMarkGreen.getFitWidth());
            clonedUnit.imageMarkGreen.setFitHeight(this.imageMarkGreen.getFitHeight());
        }

        clonedUnit.isActive = false;
        clonedUnit.maxHealth = this.maxHealth;
        numObjects++;
        System.out.println("CLONE: Unit cloned. Total objects: " + numObjects);
        clonedUnit.loadMarkImages();
        clonedUnit.setCoordinates();
        return clonedUnit;
    }

    public void attack() {
        boolean intersects = false;
        
        
        if (this.damage == null || this.damage <= 0) {
            return;
        }
        if (this.image == null) {
            return;
        }
        

        if (HelloApplication.units != null) {
            for (Unit unit : HelloApplication.units) {
                if (unit != this && unit.getTeam() != this.team && !unit.getDead() && unit.image != null) {
                    intersects = this.image.getBoundsInParent().intersects(unit.image.getBoundsInParent());
                    if (intersects) {
                        int targetHealth = unit.getHealth() == null ? 0 : unit.getHealth();
                        int newHealth = targetHealth - this.damage;
                        unit.setHealth(newHealth);
        
                        if (newHealth <= 0) {
                            unit.setHealth(0);
                            objectedKilled++;
                            unit.removeUnitFromGame();
                        }
                        
                        break;
                    }
                }
            }
        }
        if (HelloApplication.buldings != null) {
            for (World world : HelloApplication.buldings) {
                if (world != null && world.getTeam() != this.team && world.image != null) {
                    double unitWidth = this.image.getImage() != null ? this.image.getImage().getWidth() : 0;
                    double unitHeight = this.image.getImage() != null ? this.image.getImage().getHeight() : 0;
                    double worldWidth = world.image.getWidth();
                    double worldHeight = world.image.getHeight();

                    intersects = this.x < world.x + worldWidth &&
                            this.x + unitWidth > world.x &&
                            this.y < world.y + worldHeight &&
                            this.y + unitHeight > world.y;
                    if (intersects) {
                        int targetHealth = world.getHealth() == 0 ? 0 : (int) world.getHealth();
                        int newHealth = targetHealth - this.damage;
                        world.setHealth(newHealth);
                    }
                }
            }
        }
        if (!intersects) {
            return;
        }

    }

  
    public static void removeUnit() {
        numObjects--;
        System.out.println("Object removed. Total objects: " + numObjects);
    }
    public void addHealth() {
        this.health += 10;
    }
    public  void addDamage() {
        this.damage += 5;
    }
    public void changeTeam() {
        this.team = !this.team;
    }
    public boolean isAlly() {
        return this.team;
    }
    public boolean isAlly(Unit x) {
        return x != null && this.team == x.team;
    }
    public void takeDamage() {
        this.health -= 10;
        if (this.health <= 10) {
            System.out.println("Health is too low to take any damage");
        }
    }
    public void showTheStrongest(Unit unit) {
        Unit maxhealth = (this.health > unit.health) ? this : unit;
        System.out.println("The strongest unit have " + maxhealth);
    }
    public void runSetters() {
        System.out.println("Choose the option:");
        System.out.println("1. Run Pipeline");
        System.out.println("2. By default");
        Scanner scanner = new Scanner(System.in);
        int idx = scanner.nextInt();
        if  (idx == 1) {
            System.out.println("Set dead state: ");
            scanner = new Scanner(System.in);
            boolean isdead;
            isdead = scanner.nextBoolean();
            this.setDead(isdead);
            scanner.nextLine();
            System.out.println("Set health bar: ");
            int numHealth;
            numHealth = scanner.nextInt();
            this.setHealth(numHealth);
            scanner.nextLine();
            boolean spawnState;
            System.out.println("Set spawn state: ");
            spawnState = scanner.nextBoolean();
            this.setSpawned(spawnState);
            scanner.nextLine();
            System.out.println("Set team: ");
            String teamInput;
            teamInput = scanner.nextLine().trim().toLowerCase();
            boolean teamValue = teamInput.equals("ally") || teamInput.equals("true") || teamInput.equals("t");
            this.setTeam(teamValue);
            System.out.println("Set damage: ");
            int damage;
            damage = scanner.nextInt();
            this.setDamage(damage);
            scanner.nextLine();
            System.out.println("Set inventor: ");
            String inventoryInput = scanner.nextLine().trim();
            if (inventoryInput.isEmpty()) {
                this.setInventor(new ArrayList<>());
            } else {
                this.setInventor(new ArrayList<>(Arrays.asList(inventoryInput.split("\\\\s+"))));
            }
            scanner.nextLine();
        } else if (idx == 2) {
            this.setHealth(100);
            this.setSpawned(false);
            this.setTeam(true);
            this.setDamage(5);
            this.setDead(true);
            this.setInventor(new ArrayList<>(Arrays.asList("sword")));

        } else {
            System.out.println("Invalid option. Please try again.");
        }
    }
    public void runSettersToComparing(ArrayList<String> list) {
        if (list.isEmpty()) {
            System.out.println("The list is empty. Please provide some values.");
            return;
        }
        this.setHealth(null);
        this.setDamage(null);
        this.setDead(null);
        this.setSpawned(null);
        this.setInventor(null);
        Scanner scanner = new Scanner(System.in);
        for (String field : list) {
            String normalized = field.toLowerCase().trim();
            if (normalized.equals("health")) {
                scanner.nextLine();
                System.out.print("Enter health: ");
                this.setHealth(scanner.nextInt());
            } else if (normalized.equals("spawned")) {
                scanner.nextLine();
                System.out.print("Enter spawned (true/false): ");
                this.setSpawned(scanner.nextBoolean());
            } else if (normalized.equals("team")) {
                scanner.nextLine();
                System.out.print("Enter team (ally/enemy or true/false): ");
                String teamInput = scanner.nextLine().trim().toLowerCase();
                boolean teamValue = teamInput.equals("ally") || teamInput.equals("true") || teamInput.equals("t");
                this.setTeam(teamValue);
            } else if (normalized.equals("damage")) {
                scanner.nextLine();
                System.out.print("Enter damage: ");
                this.setDamage(scanner.nextInt());
            } else if (normalized.equals("dead")) {
                scanner.nextLine();
                System.out.print("Enter dead (true/false): ");
                this.setDead(scanner.nextBoolean());
            } else if (normalized.equals("inventor")) {
                scanner.nextLine();
                System.out.print("Enter inventor items (comma separated): ");
                String inventoryInput = scanner.nextLine().trim();
                if (inventoryInput.isEmpty()) {
                    this.setInventor(new ArrayList<>());
                } else {
                    String[] parts = inventoryInput.split("\\s*,\\s*");
                    this.setInventor(new ArrayList<>(Arrays.asList(parts)));
                }
            } else {
                System.out.println("Unknown characteristic: " + field + "; skipped.");
            }
        }
    }

    public int compareTo(Unit other) {
        int cmp = 0;
        if (other == null) {
            throw new IllegalArgumentException("Cannot compare to null");

        }
        if (other.getHealth() != null) {
            cmp = Integer.compare(this.health, other.health);
        }
        cmp = Boolean.compare(this.team, other.team);
        if (other.getDamage() != null) {
            cmp = this.damage.compareTo(other.damage);
        }
        if (other.getSpawned() != null) {
            cmp = this.isSpawned.compareTo(other.isSpawned);
        }
        if (other.getDead() != null) {
            cmp = this.isDead.compareTo(other.isDead);
        }
        if (other.getInventor() != null) {
            ArrayList<String> sortedInventor1 = new ArrayList<>(this.inventor);
            ArrayList<String> sortedInventor2 = new ArrayList<>(other.inventor);
            sortedInventor1.sort(String::compareTo);
            sortedInventor2.sort(String::compareTo);
            cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
        }
        return cmp;
    }

    public void print() {
        System.out.println("THE HEALTH: " + health);
        System.out.println("IF UNIT IS SPAWNED: " + isSpawned);
        System.out.println("THE DAMAGE OF THE UNIT: " + damage);
        System.out.println("THE TEAM " + (team ? "ally" : "enemy"));
        System.out.println("IF THE ENEMY ALIVE: " + isDead);
        System.out.println("THE INVENTORY: " + inventor);

    }


    
    public void resurrect() {
        if (HelloApplication.group == null || labelName == null || life == null || image == null) {
            return;
        }
        loadMarkImages();
        if (this.getTeam() == true) {
            imageMark = imageMarkGreen;
        } else {
            imageMark = imageMarkRed;
        }
        HelloApplication.group.getChildren().addAll(labelName, life, image);
        if (imageMark != null) {
            HelloApplication.group.getChildren().add(imageMark);
        }
        if (mainWeaponImage != null) {
            HelloApplication.group.getChildren().add(mainWeaponImage);
         }
        if (isActive) {
            HelloApplication.group.getChildren().add(rectActive);
        }
        setCoordinates();
        inventoryLogic();
    }
    public void loadInventoryImages() {
        try {
            if (swordImage == null && swordUrl != null) {
                swordImage = new ImageView(new Image(swordUrl.toExternalForm()));
                swordImage.setFitWidth(40);
                swordImage.setFitHeight(40);
                swordImage.setPreserveRatio(true);
            }
            if (knifeImage == null && knifeUrl != null) {
                knifeImage = new ImageView(new Image(knifeUrl.toExternalForm()));
                knifeImage.setFitWidth(40);
                knifeImage.setFitHeight(40);
                knifeImage.setPreserveRatio(true);
            }
            if (spearImage == null && spearUrl != null) {
                spearImage = new ImageView(new Image(spearUrl.toExternalForm()));
                spearImage.setFitWidth(40);
                spearImage.setFitHeight(40);
                spearImage.setPreserveRatio(true);
            }
            if (bowImage == null && bowUrl != null) {
                bowImage = new ImageView(new Image(bowUrl.toExternalForm()));
                bowImage.setFitWidth(40);
                bowImage.setFitHeight(40);
                bowImage.setPreserveRatio(true);
            }
        } catch (Exception e) {
            System.out.println("Error loading inventory images: " + e.getMessage());
        }
    }

    public void loadMarkImages() {
        if (imageMarkRed != null && imageMarkGreen != null) {
            return;
        }
        try {
            URL redUrl = getClass().getResource("/red.png");
            URL greenUrl = getClass().getResource("/green.png");
            if (redUrl != null) {
                imageMarkRed = new ImageView(new Image(redUrl.toExternalForm()));
                imageMarkRed.setFitWidth(20);
                imageMarkRed.setFitHeight(20);
            } else {
                System.out.println("Warning: red_mark.png not found in resources");
            }
            if (greenUrl != null) {
                imageMarkGreen = new ImageView(new Image(greenUrl.toExternalForm()));
                imageMarkGreen.setFitWidth(20);
                imageMarkGreen.setFitHeight(20);
            } else {
                System.out.println("Warning: green_mark.png not found in resources");
            }
        } catch (Exception e) {
            System.out.println("Error loading mark images: " + e.getMessage());
        }
    }
    public void setCoordinates() {
        if (labelName == null || life == null || image == null) {
            return;
        }
        labelName.setLayoutX(x + labelDeltaX());
        labelName.setLayoutY(y + labelDeltaY());

        double hp = getHealth() == null ? 0.0 : Math.max(0.0, getHealth());
        double effectiveMaxHealth = maxHealth > 0.0 ? maxHealth : 100.0;
        double lifeBaseX = x + lifeDeltaX();
        double lifeBaseY = y + lifeDeltaY();
        life.setStartX(lifeBaseX);
        life.setStartY(lifeBaseY);
        life.setEndX(lifeBaseX + Math.min((hp / effectiveMaxHealth) * 100, 100));
        life.setEndY(lifeBaseY);

        image.setX(x + imageDeltaX());
        image.setY(y + imageDeltaY());

        if (rectActive != null) {
            rectActive.setX(x + rectDeltaX());
            rectActive.setY(y + rectDeltaY());
        }

        if (imageMark != null) {
            imageMark.setX(x + image.getFitWidth() - 20);
            imageMark.setY(y + image.getFitHeight() - 20);
        }
        if (mainWeaponImage != null) {
            mainWeaponImage.setX(x );
            mainWeaponImage.setY(y + 10);
        }
    }
    public void move(double dx, double dy) {
        x += dx;
        y += dy;
        setCoordinates();
    }
    public void moveTo(double newX, double newY) {
        double dx = newX - x;
        double dy = newY - y;

        if (Math.abs(dx) <= moveSpeed - 3 && Math.abs(dy) <= moveSpeed - 3) {
            x = newX;
            y = newY;
        } else {
            move(dx * moveSpeed, dy * moveSpeed);
            return;
        }

        setCoordinates();
    }

    public void setPosition(double newX, double newY) {
        x = newX;
        y = newY;
        setCoordinates();
    }

    public boolean flipActivation() {
        if (HelloApplication.group != null) {
            if (isActive) {
                HelloApplication.group.getChildren().remove(rectActive);
            } else {
                HelloApplication.group.getChildren().add(rectActive);
            }
        }
        isActive = !isActive;
        return isActive;
    }

    public boolean tryActivate(double mx, double my) {
        if (image.getBoundsInParent().contains(mx, my)) {
            flipActivation();
            return true;
        }
        return false;
    }

    
    public void updateTeamMark() {
        loadMarkImages();
        if (imageMark != null && HelloApplication.group.getChildren().contains(imageMark)) {
            HelloApplication.group.getChildren().remove(imageMark);
        }
        if (this.team) {
            imageMark = imageMarkGreen;  
        } else {
            imageMark = imageMarkRed;    
        }
        if (imageMark != null) {
            HelloApplication.group.getChildren().add(imageMark);
        }
        setCoordinates();
    }
    


        protected void logic() {}

        protected void inventoryLogic() {
            // Ensure inventory images/templates are available
            loadInventoryImages();

            // Remove existing main weapon from scene if present
            if (HelloApplication.group != null && mainWeaponImage != null && HelloApplication.group.getChildren().contains(mainWeaponImage)) {
                HelloApplication.group.getChildren().remove(mainWeaponImage);
            }
            mainWeaponImage = null;

            if (this.inventor == null || this.inventor.isEmpty()) {
                setCoordinates();
                return;
            }

            String mainWeapon = this.inventor.get(0);
            int currentDamage = this.getDamage() != null ? this.getDamage() : 0;

            if (mainWeapon.equalsIgnoreCase("sword")) {
                this.setDamage(currentDamage + 5);
                if (swordImage != null && swordImage.getImage() != null) mainWeaponImage = new ImageView(swordImage.getImage());
            } else if (mainWeapon.equalsIgnoreCase("knife")) {
                this.setDamage(currentDamage + 3);
                if (knifeImage != null && knifeImage.getImage() != null) mainWeaponImage = new ImageView(knifeImage.getImage());
            } else if (mainWeapon.equalsIgnoreCase("spear")) {
                this.setDamage(currentDamage + 4);
                if (spearImage != null && spearImage.getImage() != null) mainWeaponImage = new ImageView(spearImage.getImage());
            } else if (mainWeapon.equalsIgnoreCase("bow")) {
                this.setDamage(currentDamage +2);
                if (bowImage != null && bowImage.getImage() != null) mainWeaponImage = new ImageView(bowImage.getImage());
            } else {
                // unknown main weapon -> no damage change
            }

            if (mainWeaponImage != null) {
                mainWeaponImage.setFitWidth(40);
                mainWeaponImage.setFitHeight(40);
                mainWeaponImage.setPreserveRatio(true);
                if (HelloApplication.group != null) HelloApplication.group.getChildren().add(mainWeaponImage);
            }

            // Apply secondary inventory items
            for (int i = 1; i < this.inventor.size(); i++) {
                String item = this.inventor.get(i);
                if (item == null) continue;
                String it = item.trim().toLowerCase();
                if (it.equals("health potion") || it.equals("health_potion") || it.equals("health")) {
                    this.maxHealth += 20;
                    Integer curHp = this.getHealth();
                    this.setHealth(curHp == null ? 20 : curHp + 20);
                } else if (it.equals("damage potion") || it.equals("damage_potion") || it.equals("damage")) {
                    Integer curDmg = this.getDamage();
                    this.setDamage((curDmg == null ? 0 : curDmg) + 5);
                } else {
                    // unknown supplementary item - no-op
                }
            }

            setCoordinates();
        }
}
