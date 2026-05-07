package org.example.lab4;

import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.text.Font;

import java.util.ArrayList;

public class Location {
    protected ImageView image;

    protected ArrayList<Student> belong;
    protected Label title;
    protected Label stdsProcessing;

    public void clearBelong(){
        belong = new ArrayList<>();
    }

    public boolean verifyBelong(Student st){

        if(this.intersects(st.getBoundsInParent())){
            belong.add(st);
            return true;
        }

        return false;
    }

    public boolean intersects( Bounds bounds ) {
        if( image.intersects(bounds) )return true;

        return false;
    }

    public Bounds getBoundsInParent(){
        return image.getBoundsInParent();
    }
    public Location(String t){
        title=new Label(t);
        belong = new ArrayList<>();

        stdsProcessing= new Label("0");

        HelloApplication.group.getChildren().addAll(title, stdsProcessing);
    }

    public void updateBelong(){
        stdsProcessing.setText("Processing:"+Integer.toString(belong.size()));
    }

    public void updateCache(){
        image.setOpacity(0.999);
    }

    public void postInitialization(ImageView image) {
        HelloApplication.group.getChildren().add(image) ;

        double x= image.getX();
        double y= image.getY();

        title.setFont(new Font(24));
        title.setLayoutX(x+0.0);
        title.setLayoutY(y-30.0);

        stdsProcessing.setFont(new Font(24));
        stdsProcessing.setLayoutX(x+200.0);
        stdsProcessing.setLayoutY(y-30.0);
        stdsProcessing.setText("Processing:");

    }

}
