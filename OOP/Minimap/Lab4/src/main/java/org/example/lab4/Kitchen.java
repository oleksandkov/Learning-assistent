package org.example.lab4;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Kitchen extends Location{
    public Kitchen(Image img, double x, double y){
        super(10.0, 10.0, 80.0, 80.0, 24, 6);

        image= new ImageView(img);
        HelloApplication.group.getChildren().add(image);

        image.setX(x);
        image.setY(y);

        postInitialization();
    }

}
