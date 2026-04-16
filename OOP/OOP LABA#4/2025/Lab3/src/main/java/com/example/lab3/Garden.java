package com.example.lab3;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Garden extends Location {
    private static int carrotToChew = 5;

    public Garden(Image img, double x, double y)
    {
        super(10.0, 10.0, 60, 60, 12, 6);

        image= new ImageView(img);
        World.group.getChildren().add(image);
        image.setX(x);
        image.setY(y);

        time_delay=80;
        time_current=0;

        postInitialization();
    }



}
