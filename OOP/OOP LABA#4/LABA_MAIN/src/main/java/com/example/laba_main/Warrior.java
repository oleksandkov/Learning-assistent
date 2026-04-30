package com.example.laba_main;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;


public class Warrior extends Unit {
    private static String name = "Warrior";
    private static final double MAX_HEALTH = 100.0;

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
        return 0.0;
    }

    // Getters and Setters
    public static String getName() {
        return name;
    }

    // Constructors
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

        image = new ImageView(HelloApplication.imgWarrior);
        

        isActive = false;
        rectActive = new Rectangle(x - 5, y - 5, 110, 110);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.GREEN);

        setCoordinates();
    }

  

}

