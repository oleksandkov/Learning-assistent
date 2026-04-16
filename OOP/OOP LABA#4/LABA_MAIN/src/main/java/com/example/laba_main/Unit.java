package com.example.laba_main;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Objects;
import java.util.Scanner;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Line;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;




public class Unit implements Cloneable{
    private Integer health;
    private Boolean isSpawned;
    private String Team;
    private Integer damage;
    private Boolean isDead;
    private ArrayList<String> inventor;

    private static int numObjects = 0;
    private static int objectedKilled = 0;

    // Graphical arguments
    protected Label labelName;
    protected Line life;
    protected ImageView image;
    protected double x, y; // coordinates
    protected boolean isActive;
    protected Rectangle rectActive;
    private static double MAX_HEALTH;

    static {
        System.out.println("STATIC BLOCK IS RUNT");
        System.out.println("Unit class loaded. Objects count: " + numObjects);
    }

    {
        numObjects++;
        System.out.println("INIT BLOCK IS RUNT");
        System.out.println("Object #" + numObjects + " is being initialized");
    }
    public Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this.health = health;
        this.isSpawned = isSpawned;
        Team = team;
        this.damage = damage;
        this.isDead = isDead;
        this.inventor = inventor;
        System.out.println("Constructor: Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead, ArrayList<String> inventor)");
        System.out.println("Object created. Total objects: " + numObjects);
    }

    public  Unit() {
        this(100, false, "ally", 5, false, new ArrayList<String>(Arrays.asList("sword")));
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

    public String getTeam() {
        return Team;
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
    public static class HealthComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return Integer.compare(a.getHealth(), b.getHealth());
        }
    }

    public static class TeamComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return a.getTeam().compareTo(b.getTeam());
        }
    }

    public int compareTo47(Unit x) {
        int cmp = Integer.compare(this.health, x.health);
        if (cmp != 0) return cmp;
        cmp = this.Team.compareTo(x.Team);
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
            if (template.getTeam() != null) {
                cmp = u1.getTeam().compareTo(u2.getTeam());
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


    public void setHealth(Integer health) {
        this.health = health;
        if (life != null) {
            setCoordinates();
        }
    }

    public void setSpawned(Boolean spawned) {
        isSpawned = spawned;
    }

    public void setTeam(String team) {
        Team = team;
    }

    public void setDamage(Integer damage) {
        this.damage = damage;
    }

    public void setDead(Boolean dead) {
        isDead = dead;
    }

    public void setInventor(ArrayList<String> inventor) {
        this.inventor = inventor;
    }

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return Objects.equals(health, unit.health) && Objects.equals(isSpawned, unit.isSpawned) && Objects.equals(Team, unit.Team) && Objects.equals(damage, unit.damage) && Objects.equals(isDead, unit.isDead) && Objects.equals(inventor, unit.inventor);
    }

    @Override
    public int hashCode() {
        return Objects.hash(health, isSpawned, Team, damage, isDead, inventor);
    }


    @Override
    public String toString() {
        return "Unit{" +
                "health=" + health +
                ", isSpawned=" + isSpawned +
                ", Team='" + Team + '\'' +
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
        return clonedUnit;
    }

    public void attack(Unit target) {
        if (target == null) {
            System.out.println("No target to attack.");
            return;
        }
        if (Boolean.TRUE.equals(this.isDead)) {
            System.out.println("Cannot attack: attacker is dead.");
            return;
        }
        if (Boolean.TRUE.equals(target.getDead())) {
            System.out.println("Cannot attack: target is already dead.");
            return;
        }

        if (this.damage == null || this.damage <= 0) {
            System.out.println("No damage to deal (damage is null or zero).");
            return;
        }

        int targetHealth = target.getHealth() == null ? 0 : target.getHealth();
        int newHealth = targetHealth - this.damage;
        target.setHealth(newHealth);

        if (newHealth <= 0) {
            target.setHealth(0);
            target.setDead(true);
            objectedKilled++;
            Unit.removeUnit();
            System.out.println("Target unit has been killed. Total killed: " + objectedKilled);
        } else {
            System.out.println("Target unit took " + this.damage + " damage, remaining health: " + newHealth);
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
        if (this.Team.equals("ally")) {
            this.Team = "enemy";
        } else {
            this.Team = "ally";
        }
    }
    public boolean isAlly() {
        return "ally".equals(this.Team);
    }
    public boolean isAlly(Unit x) {
        return this.Team != null && this.Team.equals(x.Team);
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
            String team;
            team = scanner.nextLine().trim();
            this.setTeam(team);
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
            this.setTeam("ally");
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
        this.setTeam(null);
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
                System.out.print("Enter team: ");
                this.setTeam(scanner.nextLine().trim());
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
        if (other.getTeam() != null) {
            cmp = this.Team.compareTo(other.Team);
        }
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
        System.out.println("THE TEAM " + Team);
        System.out.println("IF THE ENEMY ALIVE: " + isDead);
        System.out.println("THE INVENTORY: " + inventor);

    }


    // Grapphical methods
    protected double imageDeltaX() {
        return 0.0;
    }

    protected double imageDeltaY() {
        return 0.0;
    }

    public boolean isActive() {
        return isActive;
    }

    public void resurrect() {
        if (HelloApplication.group == null || labelName == null || life == null || image == null) {
            return;
        }
        HelloApplication.group.getChildren().addAll(labelName, life, image);
        if (isActive) {
            HelloApplication.group.getChildren().add(rectActive);
        }
    }
    public void setCoordinates() {
        if (labelName == null || life == null || image == null || rectActive == null) {
            return;
        }

        labelName.setLayoutX(x);
        labelName.setLayoutY(y);

        double hp = getHealth() == null ? 0.0 : Math.max(0.0, getHealth());
        life.setStartX(x);
        life.setStartY(y + 15);
        life.setEndX(x + (hp / MAX_HEALTH) * 100);
        life.setEndY(y + 15);

        image.setX(x + imageDeltaX());
        image.setY(y + imageDeltaY());

        rectActive.setX(x - 5);
        rectActive.setY(y - 5);
    }
    public void move(double dx, double dy) {
        x += dx;
        y += dy;
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

}
