package com.example.laba_main;
import java.net.URL;
import java.util.ArrayList;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;




public class Tower extends World {
    private static final double MAX_HEALTH = 300.0;
    private final ArrayList<Unit> unitsInside = new ArrayList<>();
    private int healAmount = 5;
    private Boolean team;
    private Image conturImageRedImage;
    private Image conturImageGreenImage;
    private final URL conturUrlRed = HelloApplication.class.getResource("/tower2_contour_red.png");
    private final URL conturUrlGreen = HelloApplication.class.getResource("/tower2_contour_green.png");
    


    public Tower(ArrayList<Unit> units, boolean  team) {
        super(units);
        this.team = team;
        loadContourImages();
    }

    public Tower() {
        super();
        this.team = null;
        loadContourImages();
    }

    private void loadContourImages() {
        if (conturUrlRed != null) {
            conturImageRedImage = new Image(conturUrlRed.toExternalForm(), 200, 200, false, false);
        }
        if (conturUrlGreen != null) {
            conturImageGreenImage = new Image(conturUrlGreen.toExternalForm(), 200, 200, false, false);
        }
    }

    public ArrayList<Unit> getUnitsInside() {
        return unitsInside;
    }

    public boolean  getTeam() {
        return this.team;
    }

    public void setHealAmount(int healAmount) {
        if (healAmount > 0) {
            this.healAmount = healAmount;
        }
    }

    public void setTeam(Boolean team) {
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
        if (Boolean.TRUE.equals(this.team) && conturImageGreenImage != null) {
            ImageView contourView = new ImageView(conturImageGreenImage);
            contourView.setX(x);
            contourView.setY(y);
            HelloApplication.group.getChildren().add(contourView);
        } else if (Boolean.FALSE.equals(this.team) && conturImageRedImage != null) {
            ImageView contourView = new ImageView(conturImageRedImage);
            contourView.setX(x);
            contourView.setY(y);
            HelloApplication.group.getChildren().add(contourView);
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

    public void healUnits() {
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
            if (unit.getTeam() == !this.getTeam()) {
                Integer currentHealth = unit.getHealth();
                int newHealth = (currentHealth == null ? 0 : currentHealth) - healAmount;
                unit.setHealth(newHealth);
            }
        }
    }
}

                    