package org.example.laba5;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

public class Pretorio extends Centurio {
    private static String name = "Pretorio";
    private static final double MAX_HEALTH = 150.0;
    private double  healNum = 5.0;
    private Circle areaOfEffect;
    private static final long HEALAREA_COOLDOWN = 1000;
    private long lastHealAreaTime = 0;

    private Double maxHealth = MAX_HEALTH;

    @Override
    public double getMaxHealth() {
        return maxHealth;
    }

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

    public static String getName() {
        return name;
    }

    public Pretorio(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this(health, isSpawned, team, damage, isDead, inventor, 0.0, 0.0);
        initGraphics(getName(), x, y);
    }

    public Pretorio(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead,
                    ArrayList<String> inventor, double startX, double startY) {
        super(health, isSpawned, team, damage, isDead, inventor);
        initGraphics(getName(), startX, startY);
    }

    public Pretorio() {
        this((int) MAX_HEALTH, true, true, 5, false, new ArrayList<>(Arrays.asList("Sword")), 0.0, 0.0);
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

        image = new ImageView(HelloApplication.imgPretorio);
        image.setFitWidth(100);
        image.setFitHeight(100);

        isActive = false;
        rectActive = new Rectangle(x - 5, y - 5, 110, 110);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.GREEN);
        areaOfEffect = new Circle(x, y, 150);
        areaOfEffect.setFill(Color.TRANSPARENT);
        areaOfEffect.setStroke(Color.GREEN);
        areaOfEffect.setStrokeWidth(2);
        areaOfEffect.setMouseTransparent(true);
        areaOfEffect.setVisible(false);

        setCoordinates();
    }

    @Override
    public void resurrect() {
        super.resurrect();
        if (HelloApplication.group != null && areaOfEffect != null) {
            if (!HelloApplication.group.getChildren().contains(areaOfEffect)) {
                HelloApplication.group.getChildren().add(areaOfEffect);
            }
            areaOfEffect.toBack();
            areaOfEffect.setVisible(this.isActive);

        }
        if (mainWeaponImage != null) {
            mainWeaponImage.setY(y + 17);
        }
    }

    @Override
    public void setCoordinates() {
        super.setCoordinates();
        if (areaOfEffect != null && image != null) {
            double centerX = image.getX() + image.getFitWidth() / 2.0;
            double centerY = image.getY() + image.getFitHeight() / 2.0;
            areaOfEffect.setCenterX(centerX);
            areaOfEffect.setCenterY(centerY);
        }
        if (mainWeaponImage != null) {
            mainWeaponImage.setX(x);
            mainWeaponImage.setY(y + 17);
        }
    }

    @Override
    public boolean flipActivation() {
        boolean activeNow = super.flipActivation();
        if (HelloApplication.group != null && areaOfEffect != null) {
            if (!HelloApplication.group.getChildren().contains(areaOfEffect)) {
                HelloApplication.group.getChildren().add(areaOfEffect);
            }
            areaOfEffect.setVisible(activeNow);
            areaOfEffect.toBack();
        }
        return activeNow;
    }

    private void healInArea() {
        if (HelloApplication.units != null) {
            for (Unit unit : HelloApplication.units) {
                if (unit != null
                        && unit != this
                        && unit.getTeam() == this.team
                        && unit.image != null
                        && unit.getHealth() != null
                        && unit.getHealth() > 0) {
                    double myCenterX = this.image.getBoundsInParent().getCenterX();
                    double myCenterY = this.image.getBoundsInParent().getCenterY();
                    double unitCenterX = unit.image.getBoundsInParent().getCenterX();
                    double unitCenterY = unit.image.getBoundsInParent().getCenterY();
                    
                    double dx = unitCenterX - myCenterX;
                    double dy = unitCenterY - myCenterY;
                    double distanceheal = Math.sqrt(dx * dx + dy * dy);

                    if (distanceheal < 150) {
                        int currentHealth = unit.getHealth();
                        int allyMaxHealth = (int) (unit.maxHealth > 0 ? unit.maxHealth : 100);
                        int newHealth = Math.min((int) (currentHealth + this.healNum), allyMaxHealth);
                        unit.setHealth(newHealth);
                    }
                }
            }
        }
    }
    @Override
    public void logic() {
        World mainTarget = null;
        Unit subTarget = null;
        boolean goToMain = false;
        double distanceToMainTarget = Double.MAX_VALUE;
        double distanceToSubTarget = Double.MAX_VALUE;

        if (!this.isActive()) {
            if (HelloApplication.units != null) {
                for (Unit unit : HelloApplication.units) {
                    if (unit != this && unit.getTeam() != this.team && !unit.getDead() && unit.image != null) {
                        double myCenterX = this.image.getBoundsInParent().getCenterX();
                        double myCenterY = this.image.getBoundsInParent().getCenterY();
                        double unitCenterX = unit.image.getBoundsInParent().getCenterX();
                        double unitCenterY = unit.image.getBoundsInParent().getCenterY();
                        double dx = unitCenterX - myCenterX;
                        double dy = unitCenterY - myCenterY;
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
                        double myCenterX = this.image.getBoundsInParent().getCenterX();
                        double myCenterY = this.image.getBoundsInParent().getCenterY();
                        double worldCenterX = world.imageView.getBoundsInParent().getCenterX();
                        double worldCenterY = world.imageView.getBoundsInParent().getCenterY();
                        double dx = worldCenterX - myCenterX;
                        double dy = worldCenterY - myCenterY;
                        double distancemain = Math.sqrt(dx * dx + dy * dy);

                        if (distancemain < distanceToMainTarget) {
                            distanceToMainTarget = distancemain;
                            mainTarget = world;
                        }
                    }
                }
            }

            if (mainTarget != null && subTarget != null) {
                if (distanceToMainTarget < distanceToSubTarget) {
                    goToMain = true;
                }
            } else if (mainTarget != null) {
                goToMain = true;
            } else if (subTarget != null) {
                goToMain = false;
            }

            if (goToMain && mainTarget != null) {
                this.moveTo(mainTarget.x, mainTarget.y);
            } else if (!goToMain && subTarget != null) {
                this.moveTo(subTarget.x, subTarget.y);
            }

            long currentTime = System.currentTimeMillis();
            if ((mainTarget != null || subTarget != null) && currentTime - lastAttackTime >= ATTACK_COOLDOWN) {
                attack();
                lastAttackTime = currentTime;
            }
        }

        long currentTime = System.currentTimeMillis();
        if (currentTime - lastHealAreaTime >= HEALAREA_COOLDOWN) {
            healInArea();
            lastHealAreaTime = currentTime;
        }
    }
}

