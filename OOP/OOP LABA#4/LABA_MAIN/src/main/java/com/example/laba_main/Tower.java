package com.example.laba_main;
import java.net.URL;
import java.util.ArrayList;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;




public class Tower extends World {
    private static final double MAX_HEALTH = 300.0;
    private final ArrayList<Unit> unitsInside = new ArrayList<>();
    private int healAmount = 5;
    private final URL conturUrlRed = HelloApplication.class.getResource("/red.png");
    private final URL conturUrlGreen = HelloApplication.class.getResource("/green.png");
    // private final URL conturUrlRed = HelloApplication.class.getResource("/tower2_contour_red.png");
    // private final URL conturUrlGreen = HelloApplication.class.getResource("/tower2_contour_green.png");
    


    public Tower(ArrayList<Unit> units, boolean  team) {
        super(units);
        this.team = team;
        loadContourImages();
        
    }

    public Tower() {
        super();
        this.team = false;  
        loadContourImages();
        
    }

    private void loadContourImages() {
        if (conturUrlRed != null) {
            conturImageRedImage = new Image(conturUrlRed.toExternalForm(), 20, 20, false, false);
        }
        if (conturUrlGreen != null) {
            conturImageGreenImage = new Image(conturUrlGreen.toExternalForm(), 20, 20, false, false);
        }
    }

    public ArrayList<Unit> getUnitsInside() {
        return unitsInside;
    }

    public boolean  getTeam() {
        return this.team;
    }

    private boolean containsByReference(Unit candidate) {
        for (Unit u : unitsInside) {
            if (u == candidate) {
                return true;
            }
        }
        return false;
    }

    private void removeByReference(Unit candidate) {
        unitsInside.removeIf(u -> u == candidate);
    }

    public void setHealAmount(int healAmount) {
        if (healAmount > 0) {
            this.healAmount = healAmount;
        }
    }

    public void setTeam(boolean team) {
        this.team = team;
    }

    

    @Override
    public void initGraphics(javafx.scene.image.Image image, String name, int numUnits, double x, double y, double maxHealth, double health) {
        super.initGraphics(image, name, numUnits, x, y, maxHealth, health);
        super.setMaxHealth(MAX_HEALTH);
    }

    @Override
    public void resurrectWorld() {
        super.resurrectWorld();
        loadContourImages();
        if (HelloApplication.group == null) {
            return;
        }
        if (this.team && conturImageGreenImage != null) {
            this.contourView = new ImageView(conturImageGreenImage);
            this.contourView.setX(x);
            this.contourView.setY(y);
            HelloApplication.group.getChildren().add(this.contourView);
        } else if (!this.team && conturImageRedImage != null) {
            this.contourView = new ImageView(conturImageRedImage);
            this.contourView.setX(x);
            this.contourView.setY(y);
            HelloApplication.group.getChildren().add(this.contourView);
        }
    }

    @Override
    protected void intersect() {
        ArrayList<Unit> worldUnits = getUnits();
        if ((worldUnits == null || worldUnits.isEmpty()) && HelloApplication.units != null) {
            worldUnits = HelloApplication.units;
        }
        if (worldUnits == null) {
            return;
        }

        if (this.imageView == null) {
            return;
        }

        
        unitsInside.removeIf(unit -> unit == null || Boolean.TRUE.equals(unit.getDead()));

        for (Unit unit : worldUnits) {
            if (unit == null || Boolean.TRUE.equals(unit.getDead()) || unit.image == null) {
                continue;
            }
            boolean intersects = unit.image.getBoundsInParent().intersects(this.imageView.getBoundsInParent());

            if (intersects) {
                if (!containsByReference(unit)) {
                    unitsInside.add(unit);
                }
            } else {
                removeByReference(unit);
            }
        }

        numUnits = unitsInside.size();
        if (numUnitsLabel != null) {
            numUnitsLabel.setText(String.valueOf(numUnits));
        }
    }

    public void healUnits() {
        if (this.getHealth() <= 0) {
            return;
        }
        for (Unit unit : unitsInside) {
            if (unit == null || Boolean.TRUE.equals(unit.getDead())) {
                continue;
            }

            if (unit.getTeam() == this.getTeam()) {
                Integer currentHealth = unit.getHealth();
                int newHealth = (currentHealth == null ? 0 : currentHealth) + healAmount;
                unit.setHealth(newHealth);
                
                continue;
            }
            if (unit.getTeam() != this.getTeam()) {
                Integer currentHealth = unit.getHealth();
                int newHealth = (currentHealth == null ? 0 : currentHealth) - healAmount;
                unit.setHealth(newHealth);
                
            }
        }
    }
}

                    