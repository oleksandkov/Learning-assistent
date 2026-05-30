package org.example.laba5.Unit;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import org.example.laba5.Centurio;
import org.example.laba5.Base;
import org.example.laba5.HelloApplication;
import org.example.laba5.Source;
import org.example.laba5.World;


public class Warrior extends Unit {
    private static String name = "Warrior";
    private static final double MAX_HEALTH = 100.0;
    private double oreAmount = 0;
    private int activeOre = 0;
    private static final long ORE_COOLDOWN = 1000;
    private long lastOreTime = 0;
    private boolean collectingOre = false;
    private boolean deliveringOre = false;
    private static final double COMBAT_PRIORITY_RADIUS = 180.0;


    private Label oreCountLabel;

    // When true this warrior runs inverse logic
    private boolean inverseMode = false;

    private Double maxHealth = MAX_HEALTH;
    
    @Override
    public double getMaxHealth() {
        return maxHealth;
    }
    @Override
    public Label getOreCountLabel() {
        return oreCountLabel;
    }

    @Override
    public void setOreCount(int oreCount) {
        // if (oreCount != null) {
            this.oreAmount = oreCount;
        // }
    }
    @Override
    protected double labelDeltaX() {
        return 10.0;
    }

    public double getOre() {
        return oreAmount;
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
        return 0.0;
    }

    public static String getName() {
        return name;
    }

    public Warrior(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this(health, isSpawned, team, damage, isDead, inventor, 0.0, 0.0);
        initGraphics(getName(), x, y);
    }

    public Warrior(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead,
                   ArrayList<String> inventor, double startX, double startY) {
        super(health, isSpawned, team, damage, isDead, inventor);
        initGraphics(getName(), startX, startY);
    }

    public Warrior() {
        this((int) MAX_HEALTH, true, true, 5, false, new ArrayList<>(Arrays.asList()), 0.0, 0.0);
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

        image = new ImageView(HelloApplication.imgWarrior);
        image.setFitWidth(100);
        image.setFitHeight(100);

        isActive = false;
        rectActive = new Rectangle(x - 5, y - 5, 110, 110);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.GREEN);

        oreCountLabel = new Label();
        

        setCoordinates();
    }

    @Override
    public void resurrect() {
        super.resurrect();
        if (this.getClass() == Warrior.class && oreCountLabel != null && HelloApplication.group != null) {
            if (!HelloApplication.group.getChildren().contains(oreCountLabel)) {
                HelloApplication.group.getChildren().add(oreCountLabel);
            }
        }

    }

    @Override
    public void setCoordinates() {
        super.setCoordinates();
        if (this.getClass() == Warrior.class && oreCountLabel != null) {
            oreCountLabel.setText("Ore: " + (int) activeOre);
            oreCountLabel.setLayoutX(x + 65);
            oreCountLabel.setLayoutY(y - 10 );
        }
    }

    public boolean isInverseMode() {
        return inverseMode;
    }

    public void setInverseMode(boolean inverseMode) {
        this.inverseMode = inverseMode;
    }

    private void doOreInverse() {
        if (HelloApplication.buldings == null) {
            return;
        }

        World sourceTarget = findBestSourceTarget();
        World baseTarget = getTeamBase();

        if (sourceTarget == null || baseTarget == null) {
            return;
        }

        long currentTime = System.currentTimeMillis();

        if (!collectingOre && !deliveringOre) {
            collectingOre = true;
        }

        if (collectingOre) {
            if (!this.isActive()) {
                double targetCenterX = baseTarget.imageView.getBoundsInParent().getCenterX();
                double targetCenterY = baseTarget.imageView.getBoundsInParent().getCenterY();
                moveTo(targetCenterX - this.image.getFitWidth() / 2, targetCenterY - this.image.getFitHeight() / 2);
            }
            // moveTo(sourceTarget.x, sourceTarget.y);
            if (currentTime - lastOreTime >= ORE_COOLDOWN && this.image.getBoundsInParent().intersects(baseTarget.imageView.getBoundsInParent()) ) {
                activeOre += 1;
                oreAmount += 1;
                lastOreTime = currentTime;
            }

            if (activeOre >= 10) {
                activeOre = 10;
                collectingOre = false;
                deliveringOre = true;
            }
            return;
        }

        if (deliveringOre) {
            if (!this.isActive()) {
                double targetCenterX = sourceTarget.imageView.getBoundsInParent().getCenterX();
                double targetCenterY = sourceTarget.imageView.getBoundsInParent().getCenterY();
                moveTo(targetCenterX - this.image.getFitWidth() / 2, targetCenterY - this.image.getFitHeight() / 2);
            }
            // moveTo(baseTarget.x, baseTarget.y);
            if (currentTime - lastOreTime >= ORE_COOLDOWN && this.image.getBoundsInParent().intersects(sourceTarget.imageView.getBoundsInParent()) ) {
                if (activeOre > 0) {
                    // oreAmount -= 1;
                    sourceTarget.setOre(sourceTarget.getOre() + 1);
                    activeOre -= 1;
                }
                lastOreTime = currentTime;
            }

            if (activeOre <= 0) {
                activeOre = 0;
                deliveringOre = false;
                collectingOre = true;
            }
        }
    }

    private void doOre() {
        if (HelloApplication.buldings == null) {
            return;
        }

        World sourceTarget = findBestSourceTarget();
        World baseTarget = getTeamBase();

        if (sourceTarget == null || baseTarget == null) {
            return;
        }

        long currentTime = System.currentTimeMillis();

        if (!collectingOre && !deliveringOre) {
            collectingOre = true;
        }

        if (collectingOre) {
            if (!this.isActive()) {
                double targetCenterX = sourceTarget.imageView.getBoundsInParent().getCenterX();
                double targetCenterY = sourceTarget.imageView.getBoundsInParent().getCenterY();
                moveTo(targetCenterX - this.image.getFitWidth() / 2, targetCenterY - this.image.getFitHeight() / 2);
            }
            // moveTo(sourceTarget.x, sourceTarget.y);
            if (currentTime - lastOreTime >= ORE_COOLDOWN && this.image.getBoundsInParent().intersects(sourceTarget.imageView.getBoundsInParent()) ) {
                activeOre += 1;
                oreAmount += 1;
                lastOreTime = currentTime;
            }

            if (activeOre >= 10) {
                activeOre = 10;
                collectingOre = false;
                deliveringOre = true;
            }
            return;
        }

        if (deliveringOre) {
            if (!this.isActive()) {
                double targetCenterX = baseTarget.imageView.getBoundsInParent().getCenterX();
                double targetCenterY = baseTarget.imageView.getBoundsInParent().getCenterY();
                moveTo(targetCenterX - this.image.getFitWidth() / 2, targetCenterY - this.image.getFitHeight() / 2);
            }
            // moveTo(baseTarget.x, baseTarget.y);
            if (currentTime - lastOreTime >= ORE_COOLDOWN && this.image.getBoundsInParent().intersects(baseTarget.imageView.getBoundsInParent()) ) {
                if (activeOre > 0) {
                    // oreAmount -= 1;
                    baseTarget.setOre(baseTarget.getOre() + 1);
                    activeOre -= 1;
                }
                lastOreTime = currentTime;
            }

            if (activeOre <= 0) {
                activeOre = 0;
                deliveringOre = false;
                collectingOre = true;
            }
        }
    }

    private Unit findNearbyEnemyUnit() {
        if (HelloApplication.units == null || this.image == null) {
            return null;
        }

        Unit nearest = null;
        double nearestDistance = Double.MAX_VALUE;
        double myCenterX = this.image.getBoundsInParent().getCenterX();
        double myCenterY = this.image.getBoundsInParent().getCenterY();

        for (Unit unit : HelloApplication.units) {
            if (unit == null || unit == this || unit.getDead() || unit.getTeam() == this.team || unit.image == null) {
                continue;
            }

            double unitCenterX = unit.image.getBoundsInParent().getCenterX();
            double unitCenterY = unit.image.getBoundsInParent().getCenterY();
            double dx = unitCenterX - myCenterX;
            double dy = unitCenterY - myCenterY;
            double distance = Math.sqrt(dx * dx + dy * dy);

            if (distance < COMBAT_PRIORITY_RADIUS && distance < nearestDistance) {
                nearestDistance = distance;
                nearest = unit;
            }
        }

        return nearest;
    }

    private World findNearbyEnemyBuilding() {
        if (HelloApplication.buldings == null || this.image == null) {
            return null;
        }

        World nearest = null;
        double nearestDistance = Double.MAX_VALUE;
        double myCenterX = this.image.getBoundsInParent().getCenterX();
        double myCenterY = this.image.getBoundsInParent().getCenterY();

        for (World world : HelloApplication.buldings) {
            if (world == null || world.getTeam() == this.team || world.imageView == null) {
                continue;
            }

            double worldCenterX = world.imageView.getBoundsInParent().getCenterX();
            double worldCenterY = world.imageView.getBoundsInParent().getCenterY();
            double dx = worldCenterX - myCenterX;
            double dy = worldCenterY - myCenterY;
            double distance = Math.sqrt(dx * dx + dy * dy);

            if (distance < COMBAT_PRIORITY_RADIUS && distance < nearestDistance) {
                nearestDistance = distance;
                nearest = world;
            }
        }

        return nearest;
    }

    private World findBestSourceTarget() {
        if (HelloApplication.buldings == null || this.image == null) {
            return null;
        }

        World closest = null;
        double closestDistance = Double.MAX_VALUE;
        double myCenterX = this.image.getBoundsInParent().getCenterX();
        double myCenterY = this.image.getBoundsInParent().getCenterY();

        for (World build : HelloApplication.buldings) {
            if (build == null || build.getTeam() != this.team || !(build instanceof Source) || build.imageView == null) {
                continue;
            }

            if (this.image.getBoundsInParent().intersects(build.imageView.getBoundsInParent())) {
                return build;
            }

            double targetCenterX = build.imageView.getBoundsInParent().getCenterX();
            double targetCenterY = build.imageView.getBoundsInParent().getCenterY();
            double distance = Math.hypot(targetCenterX - myCenterX, targetCenterY - myCenterY);

            if (distance < closestDistance) {
                closestDistance = distance;
                closest = build;
            }
        }

        return closest;
    }

    private World getTeamBase() {
        if (HelloApplication.basesA == null || HelloApplication.basesB == null) {
            return null;
        }

        ArrayList<Base> bases = this.team ? HelloApplication.basesA : HelloApplication.basesB;
        if (bases == null || bases.isEmpty()) {
            return null;
        }

        Base base = bases.get(0);
        if (base == null || base.imageView == null) {
            return null;
        }

        return base;
    }

    @Override
    protected void promotion() {
        if (HelloApplication.units == null) {
            return;
        }
        if (this != null )  {
            int ore = (int) this.getOre();
            if (ore >= 100) {
                this.setDead(true);
                HelloApplication.group.getChildren().removeAll(this.image, this.labelName, this.life, this.rectActive, oreCountLabel);
                this.removeUnitFromGame();
                Centurio centurio = new Centurio((int) MAX_HEALTH, true, this.team, 10, false, new ArrayList<>(this.getInventor()), this.x, this.y);
                HelloApplication.units.add(centurio);
                centurio.resurrect();
                
            }   

        }
    }
    
    
    public void attackInverse() {
         boolean intersects = false;

        if (this.getDamage() == null || this.getDamage() <= 0) {
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
                        int newHealth = targetHealth + this.getDamage();
                        unit.setHealth(newHealth);

                        System.out.println("[Combat Inverse] " + this.getClass().getSimpleName() + " (" + (this.team ? "Ally" : "Enemy") + 
                                           ") healed " + unit.getClass().getSimpleName() + 
                                           " (" + (unit.team ? "Ally" : "Enemy") + ") for " + this.getDamage() + " HP. Target HP: " + newHealth);

                        if (newHealth <= 0) {
                            unit.setHealth(0);
                            plusObjectedKilled();
                            unit.removeUnitFromGame();
                        }

                        break;
                    }
                }
            }
        }
        if (HelloApplication.buldings != null) {
            for (World world : HelloApplication.buldings) {
                if (world != null && world.getTeam() != this.team && world.imageView != null) {
                    intersects = this.image.getBoundsInParent().intersects(world.imageView.getBoundsInParent());
                    if (intersects) {
                        int targetHealth = world.getHealth() == 0 ? 0 : (int) world.getHealth();
                        int newHealth = targetHealth + this.getDamage();
                        world.setHealth(newHealth);
                        System.out.println("[Combat Inverse] " + this.getClass().getSimpleName() + " (" + (this.team ? "Ally" : "Enemy") + 
                                           ") healed Building " + world.name + 
                                           " (" + (world.team ? "Ally" : "Enemy") + ") for " + this.getDamage() + " HP. Target HP: " + newHealth);
                    }
                }
            }
        }
        if (!intersects) {
            return;
        }
    }
    
    @Override
    public void logic() {
        Unit nearbyEnemyUnit = findNearbyEnemyUnit();
        World nearbyEnemyBuilding = findNearbyEnemyBuilding();

        if (!this.isActive() && (nearbyEnemyUnit != null || nearbyEnemyBuilding != null)) {
            if (nearbyEnemyBuilding != null) {
                double bCenterX = nearbyEnemyBuilding.imageView.getBoundsInParent().getCenterX();
                double bCenterY = nearbyEnemyBuilding.imageView.getBoundsInParent().getCenterY();
                double myCenterX = this.image.getBoundsInParent().getCenterX();
                double myCenterY = this.image.getBoundsInParent().getCenterY();
                double dist = Math.hypot(bCenterX - myCenterX, bCenterY - myCenterY);
                if (dist > 130.0) {
                    this.moveTo(bCenterX - this.image.getFitWidth() / 2, bCenterY - this.image.getFitHeight() / 2);
                }
            } else {
                double uCenterX = nearbyEnemyUnit.image.getBoundsInParent().getCenterX();
                double uCenterY = nearbyEnemyUnit.image.getBoundsInParent().getCenterY();
                double myCenterX = this.image.getBoundsInParent().getCenterX();
                double myCenterY = this.image.getBoundsInParent().getCenterY();
                double dist = Math.hypot(uCenterX - myCenterX, uCenterY - myCenterY);
                if (dist > 70.0) {
                    this.moveTo(uCenterX - this.image.getFitWidth() / 2, uCenterY - this.image.getFitHeight() / 2);
                }
            }

            long currentTime = System.currentTimeMillis();
            if (currentTime - lastAttackTime >= ATTACK_COOLDOWN) {
                attack();
                lastAttackTime = currentTime;
            }
            return;
        }

        // if (!this.isActive()) {
            doOre();
        // }
        promotion();
    }

    
    @Override
    public void logicInverse() {
        Unit nearbyEnemyUnit = findNearbyEnemyUnit();
        World nearbyEnemyBuilding = findNearbyEnemyBuilding();

        if (!this.isActive() && (nearbyEnemyUnit != null || nearbyEnemyBuilding != null)) {
            if (nearbyEnemyBuilding != null) {
                double bCenterX = nearbyEnemyBuilding.imageView.getBoundsInParent().getCenterX();
                double bCenterY = nearbyEnemyBuilding.imageView.getBoundsInParent().getCenterY();
                double myCenterX = this.image.getBoundsInParent().getCenterX();
                double myCenterY = this.image.getBoundsInParent().getCenterY();
                double dist = Math.hypot(bCenterX - myCenterX, bCenterY - myCenterY);
                if (dist > 130.0) {
                    this.moveTo(bCenterX - this.image.getFitWidth() / 2, bCenterY - this.image.getFitHeight() / 2);
                }
            } else {
                double uCenterX = nearbyEnemyUnit.image.getBoundsInParent().getCenterX();
                double uCenterY = nearbyEnemyUnit.image.getBoundsInParent().getCenterY();
                double myCenterX = this.image.getBoundsInParent().getCenterX();
                double myCenterY = this.image.getBoundsInParent().getCenterY();
                double dist = Math.hypot(uCenterX - myCenterX, uCenterY - myCenterY);
                if (dist > 70.0) {
                    this.moveTo(uCenterX - this.image.getFitWidth() / 2, uCenterY - this.image.getFitHeight() / 2);
                }
            }

            long currentTime = System.currentTimeMillis();
            if (currentTime - lastAttackTime >= ATTACK_COOLDOWN) {
                attackInverse();
                lastAttackTime = currentTime;
            }
            return;
        }

        // if (!this.isActive()) {
            doOreInverse();
        // }
        promotion();
    }

    @Override
    public Unit clone() throws CloneNotSupportedException {
        Warrior cloned = (Warrior) super.clone();
        cloned.oreAmount = this.oreAmount;
        cloned.activeOre = this.activeOre;
        cloned.collectingOre = this.collectingOre;
        cloned.deliveringOre = this.deliveringOre;
        cloned.lastOreTime = this.lastOreTime;
        cloned.inverseMode = this.inverseMode;
        if (this.oreCountLabel != null) cloned.oreCountLabel = new Label(this.oreCountLabel.getText());
        return cloned;
    }
}
