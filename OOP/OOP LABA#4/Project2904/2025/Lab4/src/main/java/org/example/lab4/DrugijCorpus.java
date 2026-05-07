package org.example.lab4;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class DrugijCorpus extends Location {
    public DrugijCorpus(Image img, double x, double y){
        super("Другий корпус");

        image= new ImageView(img);
        //HelloApplication.group.getChildren().add(image);

        image.setX(x);
        image.setY(y);

        postInitialization(image);
    }

}
