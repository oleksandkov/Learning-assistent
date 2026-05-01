package com.example.laba_main;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

public class Centurio extends Warrior{
    private static String name = "Centurio";
    private static final double MAX_HEALTH = 120.0;
    private double healNum = 10.0;
    private static final long HEAL_COOLDOWN = 2000; 
    private long lastHealTime = 0;

     @Override
    protected double labelDeltaX() {
        return 10.0;
    }

    @Override
    protected double labelDeltaY() {
        return -10.0;
    }

    @Override
    protected double lifeDeltaX() {
        return 0.0;
    }

    @Override
    protected double lifeDeltaY() {
        return 10.0;
    }

    @Override
    protected double rectDeltaX() {
        return -9.0;
    }

    @Override
    protected double rectDeltaY() {
        return -9.0;
    }

    @Override
    protected double imageDeltaX() {
        return 0.0;
    }

    @Override
    protected double imageDeltaY() {
        return 12.0;
    }

    @Override
    public void setCoordinates() {
        super.setCoordinates();
        // Centurio-specific weapon offset must be applied on every coordinate update.
        if (mainWeaponImage != null) {
            mainWeaponImage.setX(x);
            mainWeaponImage.setY(y + 17);
        }
    }

    // Getters and Setters
    public static String getName() {
        return name;
    }

    // Constructors
    public Centurio(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this(health, isSpawned, team, damage, isDead, inventor, 0.0, 0.0);
        initGraphics(getName(), x, y);
    }

    public Centurio(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead,
                   ArrayList<String> inventor, double startX, double startY) {
        super(health, isSpawned, team, damage, isDead, inventor);
        initGraphics(getName(), startX, startY);
    }

    public Centurio() {
        this(100, true, true, 5, false, new ArrayList<>(Arrays.asList("Knife")), 0.0, 0.0);
        initGraphics(getName(), x, y);
    }

    private void initGraphics(String name, double startX, double startY) {
        this.x = startX;
        this.y = startY;
        setMaxHealth(MAX_HEALTH);

        this.labelName = new Label(name);

        life = new Line();
        life.setStrokeWidth(5);
        life.setStroke(Color.LIGHTGREEN);

        image = new ImageView(HelloApplication.imgCenturio);
        image.setFitWidth(100);
        image.setFitHeight(100);

        isActive = false;
        rectActive = new Rectangle(x - 5, y - 5, 110, 110);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.GREEN);

        setCoordinates();
    }
    
    

    private void heal(World target) {
        if (target == null || target.imageView == null || this.image == null) {
            return;
        }
        double myCenterX = this.image.getBoundsInParent().getCenterX();
        double myCenterY = this.image.getBoundsInParent().getCenterY();
        double targetCenterX = target.imageView.getBoundsInParent().getCenterX();
        double targetCenterY = target.imageView.getBoundsInParent().getCenterY();
        
       boolean isIntersecting = Math.sqrt(Math.pow(targetCenterX - myCenterX, 2) + Math.pow(targetCenterY - myCenterY, 2)) < 100;

        if (isIntersecting) {
            double newHealth = Math.min(target.getHealth() + this.healNum, target.getMaxHealth());
            target.setHealth(newHealth);
        }
    }

    @Override
    public void logic() {
        World mainTarget = null;
        Unit subTarget = null;
        World healTarget = null;
        boolean goToMain = false;
        double distanceToMainTarget = Double.MAX_VALUE;
        double distanceToSubTarget = Double.MAX_VALUE;
        double distanceToHealTarget = Double.MAX_VALUE;

     
        if (HelloApplication.units != null && !this.isActive()) {
            for (Unit unit : HelloApplication.units) {
                if (unit != this && unit.getTeam() != this.team && !unit.getDead() && unit.image != null) {
                    double dx = (unit.x + unit.image.getFitWidth() / 2) - (this.x + this.image.getFitWidth() / 2);
                    double dy = (unit.y + unit.image.getFitHeight() / 2) - (this.y + this.image.getFitHeight() / 2);
                    double distancesub = Math.sqrt(dx * dx + dy * dy);
                    if (distancesub < distanceToSubTarget) {
                        distanceToSubTarget = distancesub;
                        subTarget = unit;
                    }
                }
            }
        }

        if (HelloApplication.buldings != null) {
            for (World world : HelloApplication.buldings) {
                if (world != null && world.getTeam() != this.team && world.imageView != null) {
                    double dx = (world.x + world.imageView.getFitWidth() / 2) - (this.x + this.image.getFitWidth() / 2);
                    double dy = (world.y + world.imageView.getFitHeight() / 2) - (this.y + this.image.getFitHeight() / 2);
                    double distancemain = Math.sqrt(dx * dx + dy * dy);
                    if (distancemain < distanceToMainTarget) {
                        distanceToMainTarget = distancemain;
                        mainTarget = world;
                    }
                }
            }
        }

        if (HelloApplication.buldings != null) {
            for (World world : HelloApplication.buldings) {
                if (world != null && world.getTeam() == this.team && world.imageView != null && world.getHealth() < world.getMaxHealth()) {
                    double myCenterX = this.image.getBoundsInParent().getCenterX();
                    double myCenterY = this.image.getBoundsInParent().getCenterY();
                    double worldCenterX = world.imageView.getBoundsInParent().getCenterX();
                    double worldCenterY = world.imageView.getBoundsInParent().getCenterY();
                    
                    double dx = worldCenterX - myCenterX;
                    double dy = worldCenterY - myCenterY;
                    double distanceheal = Math.sqrt(dx * dx + dy * dy);
                    if (distanceheal < distanceToHealTarget && world.team == this.team && world.getHealth() < world.getMaxHealth() / 2) {
                        distanceToHealTarget = distanceheal;
                        healTarget = world;
                    }
                }
            }
        }

        // Healing first
        if (!this.isActive()) {
            if (healTarget != null) {
                this.moveTo(healTarget.x + healTarget.imageView.getFitWidth() / 2, healTarget.y + healTarget.imageView.getFitHeight() / 2);
                long currentTime = System.currentTimeMillis();
                if (currentTime - lastHealTime >= HEAL_COOLDOWN) {
                    heal(healTarget);
                    lastHealTime = currentTime;
                }
                if (healTarget.getHealth() >= healTarget.getMaxHealth()) {
                    healTarget = null; 
                }
                return; 
            }

            // Then handle combat
            if (mainTarget != null && subTarget != null) {
                goToMain = distanceToMainTarget < distanceToSubTarget;
            } else if (mainTarget != null) {
                goToMain = true;
            } else if (subTarget != null) {
                goToMain = false;
            } else {
                return; 
            }

            if (goToMain) {
                this.moveTo(mainTarget.x, mainTarget.y);
            } else {
                this.moveTo(subTarget.x, subTarget.y);
            }
        
            long currentTime = System.currentTimeMillis();
            if (currentTime - lastAttackTime >= ATTACK_COOLDOWN) {
                attack();
                lastAttackTime = currentTime;
            }
        }
    }

   
}
