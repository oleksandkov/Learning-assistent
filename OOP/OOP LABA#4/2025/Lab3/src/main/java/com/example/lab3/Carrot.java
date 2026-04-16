package com.example.lab3;

import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;

import java.util.Random;

public class Carrot {
    private Label food;
    public static final int foodMax=100;
    private ImageView icarrot;

    public Carrot(double screenWX, double screenWY){
        int f = World.rand.nextInt(foodMax)+1;

        double x= World.rand.nextDouble()*screenWX;
        double y= World.rand.nextDouble()*screenWY;

        food = new Label(Integer.toString(f));
        food.setLayoutX(x);
        food.setLayoutY(y);

        icarrot = new ImageView(World.imgcarrot);
        icarrot.setX(x);
        icarrot.setY(y+15);

        World.group.getChildren().addAll( food, icarrot);
    }

    public void harakiri(){
        World.group.getChildren().removeAll( food, icarrot);
    }
    public Bounds getBoundsInParent(){
        return icarrot.getBoundsInParent();
    }
    public int getFood(){
        String str= food.getText();
        int f= Integer.parseInt(str);
        return f;
    }

}
