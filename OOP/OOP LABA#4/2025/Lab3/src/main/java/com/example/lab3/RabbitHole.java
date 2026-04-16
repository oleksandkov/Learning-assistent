package com.example.lab3;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class RabbitHole extends Location {
    public RabbitHole(Image img, double x, double y)
    {
        super(10.0, 10.0, 60, 60, 24, 6);

        image= new ImageView(img);
        World.group.getChildren().add(image);
        image.setX(x);
        image.setY(y);

        time_delay=60;
        time_current=0;

        postInitialization();
    }

    public void lifeCycle()
    {
        time_current++;
        if(time_current<time_delay)return;
        time_current=0;
//                    accountOperated();
        for( int i=0; i<maxRabbits; i++ )
        {
            if( places[i]!=null)
            {
                Rabbit rbt=places[i];

                if( places[i].isHungry() ){
                    clearPlace(i);
                    World.askWorldplanning(rbt, Operations.FINDFOOD);
                    return;
                }

                places[i].tryToHeal();

                if( places[i].isFullHealth() ){
                    clearPlace(i);
                    World.askWorldplanning(rbt, Operations.FINDFOOD);
                    return;
                }
            }
        }
    }

    @Override
    public String getName() {
        return "RabbitHole";
    }
}
