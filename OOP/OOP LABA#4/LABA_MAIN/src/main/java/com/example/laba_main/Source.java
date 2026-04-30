package com.example.laba_main;
import java.net.URL;
import java.util.ArrayList;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;




public class Source extends World {
    private static final double MAX_HEALTH = 200.0;
    private final ArrayList<Unit> unitsInside = new ArrayList<>();
    private final URL conturUrlRed = HelloApplication.class.getResource("/red.png");
    private final URL conturUrlGreen = HelloApplication.class.getResource("/green.png");
    


    public Source(ArrayList<Unit> units, boolean  team) {
        super(units);
        this.team = team;
        loadContourImages();
        
    }

    public Source() {
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
        
        if (this.imageView != null) {
            this.imageView.setY(y + 7);
        }
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

        // Remove dead units from unitsInside
        unitsInside.removeIf(unit -> unit == null || Boolean.TRUE.equals(unit.getDead()));

        for (Unit unit : worldUnits) {
            if (unit == null || Boolean.TRUE.equals(unit.getDead())) {
                continue;
            }
            double unitX = unit.x;
            double unitY = unit.y;
            double unitWidth = unit.image != null ? unit.image.getImage().getWidth() : 0;
            double unitHeight = unit.image != null ? unit.image.getImage().getHeight() : 0;

            double towerX = this.x;
            double towerY = this.y;
            double towerWidth = this.image != null ? this.image.getWidth() : 0;
            double towerHeight = this.image != null ? this.image.getHeight() : 0;

            boolean intersects = unitX < towerX + towerWidth &&
                    unitX + unitWidth > towerX &&
                    unitY < towerY + towerHeight &&
                    unitY + unitHeight > towerY;

            if (intersects) {
                if (!unitsInside.contains(unit)) {
                    unitsInside.add(unit);
                }
            } else {
                unitsInside.remove(unit);
            }
        }

        numUnits = unitsInside.size();
        if (numUnitsLabel != null) {
            numUnitsLabel.setText(String.valueOf(numUnits));
        }
    }

    
}