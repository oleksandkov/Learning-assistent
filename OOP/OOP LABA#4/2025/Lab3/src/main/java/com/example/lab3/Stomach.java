package com.example.lab3;

import javafx.scene.control.Label;
import javafx.scene.paint.Color;

public class Stomach implements Cloneable {
    Label volume;
    public static double labelDeltaX(){ return 85.0; }
    public static double labelDeltaY(){ return 0.0; }

    private static final int maxVolume= 200;

    private static int physiologyTickMax = 50;
    private int physiologyTicks;
//    Глибинне копіювання
    @Override
    protected Object clone() throws CloneNotSupportedException {
        String str = this.volume.getText();
        int f = Integer.parseInt(str);
        double x=volume.getLayoutX();
        double y=volume.getLayoutY();

        Stomach tmp = new Stomach(f, x, y);
        return tmp;
    }

//    Поверхневе копіювання
//    @Override
//    protected Object clone() throws CloneNotSupportedException {
//
//    return super.clone();
//    }


    public Stomach(int f, double x, double y){
        String str= Integer.toString(f);

        volume= new Label(str);
        volume.setLayoutX(x+labelDeltaX());
        volume.setLayoutY(y+labelDeltaY());
        volume.setTextFill(Color.RED);

        physiologyTicks=0;

    }

    public void setCoordinates(double x, double y){
        volume.setLayoutX(x+labelDeltaX());
        volume.setLayoutY(y+labelDeltaY());
    }
    public void eat(int food){
        String str = volume.getText();
        int f = Integer.parseInt(str);
        f=f+food;
        str=Integer.toString(f);
        volume.setText(str);
    }
    public Label getLabel(){
        return volume;
    }

    public boolean isHungry(){
        if( ((double)(getFood())/(double)(maxVolume)) < 0.33 )return true;

        return false;
    }
    public boolean isFullStomach(){
      if( getFood() < maxVolume )return false;

      return true;
    }

    public int getFood(){
        String str = volume.getText();
        int f = Integer.parseInt(str);
        return f;
    }


    public boolean useEnergy(){
        ++physiologyTicks;
        if( physiologyTicks==physiologyTickMax){
            physiologyTicks=0;
            int food=getFood();
            if(food>0){
                eat(-1);
                return true;
            }
            else return false;
        }
        return true;
    }
}
