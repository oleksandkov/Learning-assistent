package com.example.lab3;

import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

public class Wolf {
    private Label name;
    private Stomach stomach;
    private double health;
    private static double healthMax=100;
    private Line life;
    private ImageView iwolf;
    private double x, y;

    private double aimx;
    private double aimy;

    public static double imageDeltaX(){
        return 0.0;
    }
    public static double imageDeltaY(){
        return 15+7;
    }
    public Wolf(String n, double h, double _x, double _y ){
        x=_x;
        y=_y;

        clearAim();

        name=new Label(n);
        name.setLayoutX(x);
        name.setLayoutY(y);
        name.setTextFill(Color.RED);

        stomach=new Stomach(0,x,y);

        health=h;
        life=new Line(x,y+15, x+health,y+15);
        life.setStrokeWidth(5);
        life.setStroke(Color.LIGHTGREEN);

        iwolf=new ImageView(World.imgwolf);
        iwolf.setX(x+imageDeltaX());
        iwolf.setY(y+imageDeltaY());



    }
    public double speedX(){
        return 6.0;
    }
    public double speedY(){
        return 6.0;
    }
    public void clearAim()
    {
        aimx = aimy = -1000.0;
    }

    public void resurrect(){
        World.group.getChildren().addAll(name, life, iwolf, stomach.getLabel());

    }

    public void harakiri(){
        World.group.getChildren().removeAll(name, life, iwolf, stomach.getLabel());

    }

    public boolean isEmptyAim()
    {
        if( (aimx<0) && (aimy<0) )return true;

        return false;
    }

    public void autoMove(){
        //double x = imgv.getX();
        //double y = imgv.getY();

        if( isEmptyAim() ) {
            //System.out.println("Wolf.isEmptyAim()");
            World.askWorldwhatToDo(this);
        }
        else {


            if ((Math.abs(x - aimx) + Math.abs(y - aimy)) < 1.0) {
                clearAim();
                //System.out.println("Wolf.clearAim()");
            } else {

                double signdx = Math.signum(aimx - x);
                double dx = Math.abs(aimx - x);
                dx = ((dx < speedX()) ? dx : speedX());
                dx = signdx * dx;

                double signdy = Math.signum(aimy - y);
                double dy = Math.abs(aimy - y);
                dy = ((dy < speedY()) ? dy : speedY());
                dy = signdy * dy;
                //------------------------------------------

                double sav_x =x;
                double sav_y =y;

                //System.out.println("move(dx, dy);");
                move(dx, dy);

                if( !World.wolfRestrictionsSatisfied( getBoundsInParent() ) ){
                    clearAim();
                }


            }
        }
    }

    public void move( double dx, double dy ){

        x=x+dx;
        y=y+dy;
        setCoordinates();
    }
    public void setCoordinates(){
        name.setLayoutX(x);
        name.setLayoutY(y);

        stomach.setCoordinates(x, y);

        life.setStartX(x);
        life.setStartY(y+15);
        life.setEndX(x+health);
        life.setEndY(y+15);

        //        World.group.getChildren().remove(life);
//        life=new Line(x,y+15, x+health,y+15);
//        life.setStrokeWidth(5);
//        life.setStroke(Color.LIGHTGREEN);
//        World.group.getChildren().add(life);

        iwolf.setX(x+imageDeltaX());
        iwolf.setY(y+imageDeltaY());


    }

    public void setAim( double ax, double ay ){
        aimx = ax;
        aimy = ay;
        //System.out.println("setAim( double ax, double ay )"+ax+" "+ay);
    }

    public Bounds getBoundsInParent(){
        return iwolf.getBoundsInParent();
    }

}
