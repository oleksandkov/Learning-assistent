package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class GOK extends Location {

    public boolean intersects( double x, double y, double wx, double wy ) {
        if( image.intersects(x,y,wx,wy) )return true;

        return false;
    }

    public void lifeCycle() {
        time_current++;
        if (time_current < time_delay) return;
        time_current = 0;

        int minedOre= World.mineOre();

        for( int i=0; i<maxcars; i++ ) {
            if( places[i]!=null) {
                if(!places[i].carryWeight(WeightType.ORE))
                {
                    LightCar car=places[i];
                    clearPlace(i);

                    Main.world.planning(car, Operations.GETSOMEREST);
                }
                else
                {
                    minedOre= places[i].load(WeightType.ORE, minedOre);

                    if( places[i].isFull() ) {
                    LightCar car = places[i];
                    clearPlace(i);

                    Main.world.planning(car, Operations.CARRYORE);
                    accountOperated();
                    }
                }
            }
            if( minedOre==0)break;
        }
    }
    public GOK(Image img, double x, double y)
    {
        super(410, 0, 0, 55, 10, 1, 0);

        image= new ImageView(img);
        Main.group.getChildren().add(image);

        image.setX(x);
        image.setY(y);

        time_delay=40;
        time_current=0;

        postInitialization();
    }

}
