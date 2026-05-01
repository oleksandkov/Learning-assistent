package com.example.laba_main;
import java.util.ArrayList;
import java.util.Iterator;

import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.shape.Line;


public class World {
    private ArrayList<Unit> units;

    protected int x;
    protected int y;
    protected Image image;
    protected String name;
    protected int numUnits;
    protected Label labelName;
    protected Label numUnitsLabel;
    protected Line life;
    protected double health;
    protected boolean team;


    
    protected Image conturImageRedImage;
    protected Image conturImageGreenImage;
    protected ImageView contourView;
    protected ImageView imageView;
    protected double maxHealth;

    protected double oreAmount = 0;

    public double getOre() {
        return this.oreAmount;
    }

    public void setOre(double oreAmount) {
        this.oreAmount = oreAmount;
    }

    protected void setMaxHealth(double maxHealth) {
        this.maxHealth = maxHealth;
    }

    protected void setHealth(double health) {
        this.health = health;
        updateLifeBar();
    }
    protected double getHealth() {
        return health;
    }
    protected double getMaxHealth() {
        return maxHealth;
    }
    protected boolean getTeam() {
        return team;
    }

    protected void updateLifeBar() {
        if (life == null) {
            return;
        }

        
        double effectiveMaxHealth = maxHealth > 0.0 ? maxHealth : 100.0;
        double currentHealth = Math.max(0.0, Math.min(health, effectiveMaxHealth));
        double imageHeight = image != null && image.getHeight() > 0 ? image.getHeight() : 100.0;
        double barWidth = (currentHealth / effectiveMaxHealth) * imageHeight;

        double lifeBaseX = x - 5;
        double lifeBaseY = y + 5;
        life.setStartX(lifeBaseX);
        life.setStartY(lifeBaseY);
        life.setEndX(lifeBaseX );
        life.setEndY(lifeBaseY + barWidth);
    }



    public static int objects = Unit.getNumObjects();

    public World() {
        units = new ArrayList<>();
    }

    public World(ArrayList<Unit> units) {
        this.units = units;
    }

    public ArrayList<Unit> getUnits() {
        return units;
    }

    public void setUnits(ArrayList<Unit> units) {
        this.units = units;
    }

    public void printUnits() {
        if (units == null || units.isEmpty()) {
            System.out.println("No units in this object.");
            return;
        }
        for (int i = 0; i < units.size(); i++) {
            System.out.println("Unit " + i + ":");
            System.out.println(units.get(i));
        }
    }

    public void insertUnit(int index, Unit unit) {
        if (units == null) {
            units = new ArrayList<>();
        }
        units.add(index, unit);
    }



    public void update() {
        if (units != null) {
            Iterator<Unit> it = units.iterator();
            while (it.hasNext()) {
                Unit u = it.next();
                if (u == null || Boolean.TRUE.equals(u.getDead()) || (u.getHealth() != null && u.getHealth() <= 0)) {
                    it.remove();
                    Unit.removeUnit();
                }
            }
        }
    }

    

  

   

    
    protected void initGraphics(Image image, String name, int numUnits, double x, double y, double maxHealth, double health) {
        this.image = image;
        this.name = name;
        this.numUnits = numUnits;
        this.labelName = new Label(name);
        this.numUnitsLabel = new Label(String.valueOf(numUnits));
        this.life = new Line();
        this.x = (int) x;
        this.y = (int) y;

        setMaxHealth(maxHealth);
        setHealth(health);

        life.setStrokeWidth(5);
        life.setStroke(javafx.scene.paint.Color.GREEN);
        updateLifeBar();

        labelName.setLayoutX(x);
        labelName.setLayoutY(y - 30);
        labelName.setFont(javafx.scene.text.Font.font(20));

        numUnitsLabel.setLayoutX(x + image.getWidth() + 7);
        numUnitsLabel.setLayoutY(y);
        numUnitsLabel.setFont(javafx.scene.text.Font.font(20));
    }
    protected void resurrectWorld() {
        if (HelloApplication.group == null || labelName == null || life == null || image == null) {
            return;
        }
        imageView = new javafx.scene.image.ImageView(image);
        imageView.setX(x);
        imageView.setY(y);
        HelloApplication.group.getChildren().addAll(labelName, life, imageView, numUnitsLabel);
        
    }
    protected void removeBuildingFromGame() {
        if (HelloApplication.group == null) {
            return;
        }
        if (labelName != null) {
            HelloApplication.group.getChildren().remove(labelName);
        }
        if (life != null) {
            HelloApplication.group.getChildren().remove(life);
        }
        if (imageView != null) {
            HelloApplication.group.getChildren().remove(imageView);
        }
        if (numUnitsLabel != null) {
            HelloApplication.group.getChildren().remove(numUnitsLabel);
        }
        if (conturImageGreenImage != null && this.team) {
            HelloApplication.group.getChildren().remove(contourView);
        }
        if (conturImageRedImage != null && !this.team) {
            HelloApplication.group.getChildren().remove(contourView);
        }
        HelloApplication.buldings.remove(this);
        if (this.getClass().getSimpleName().equals("Base") && this.getTeam()== true) {
            HelloApplication.basesA.remove(this);
        } else if (this.getClass().getSimpleName().equals("Base") && this.getTeam()== false) {
            HelloApplication.basesB.remove(this);
        } else if (this.getClass().getSimpleName().equals("Tower") && this.getTeam()== true) {
            HelloApplication.towersA.remove(this);
        } else if (this.getClass().getSimpleName().equals("Tower") && this.getTeam()== false) {
            HelloApplication.towersB.remove(this);
        } else if (this.getClass().getSimpleName().equals("Source") && this.getTeam()== true) {
            HelloApplication.sourcesA.remove(this);
        } else if (this.getClass().getSimpleName().equals("Source") && this.getTeam()== false) {
            HelloApplication.sourcesB.remove(this);
        }

        System.out.println("Building removed. Remaining buildings: " + HelloApplication.buldings.size());

    }


    protected void intersect() {}
    
}

