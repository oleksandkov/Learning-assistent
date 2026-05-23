package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Baza extends Location{

    public boolean intersects( double x, double y, double wx, double wy ) {
        if( image.intersects(x,y,wx,wy) )return true;

        return false;
    }


    public Baza(Image img, double x, double y)
    {
        super(0, -70.0, 105, 0, 5, 1, 0);

        image= new ImageView(img);

        Main.group.getChildren().add(image);

        image.setX(x);
        image.setY(y);

        time_delay=40;
        time_current=0;

        postInitialization();
    }

    public int searchSteel() {
        for (int i = 0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if( places[i].getWeightType()==WeightType.STEEL ) return i;
            }
        }

        return -1;
    }


    public void lifeCycle() {
        time_current++;
        if (time_current < time_delay) return;
        time_current = 0;

        for(int i=0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if( places[i].getWeightType()!=WeightType.STEEL )
                {
                    LightCar car = places[i];
                    clearPlace(i);
                    Main.world.planning(car, Operations.GETSOMEREST);
                }
            }
        }

        int isteel= searchSteel(); //индекс грузовика для погрузки стали

        if( isteel==-1 )return;

        int soldSteel = places[isteel].getMass();

        soldSteel= (  soldSteel< World.soldSteelLimit() )?
                soldSteel:World.soldSteelLimit() ;

        places[isteel].unload(WeightType.STEEL,soldSteel);

        if(places[isteel].getMass()==0)accountOperated();

        World.soldSteel(soldSteel);

    }
}
