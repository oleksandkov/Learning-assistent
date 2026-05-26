package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Shahta extends Location {

   public boolean intersects( double x, double y, double wx, double wy ) {
       if( image.intersects(x,y,wx,wy) )return true;

       return false;
   }


    public Shahta(Image img, double x, double y)
    {
        super(410, 0, 0, 55, 10, 2, 0);
        image= new ImageView(img);
        Main.group.getChildren().add(image);
        image.setX(x);
        image.setY(y);

        miniImage= new ImageView(img);
        miniImage.setPreserveRatio(true);
        miniImage.setFitHeight(60.0);
        Main.miniGroup.getChildren().add(miniImage);

        miniImage.setX(x*Main.SCALE);
        miniImage.setY(y*Main.SCALE);


        time_delay=40;
        time_current=0;

        postInitialization();
    }

    public void lifeCycle()
    {
        time_current++;
        if(time_current<time_delay)return;
        time_current=0;

        int minedCoal= World.mineCoal();
//        System.out.println("int minedCoal= World.mineCoal();"+minedCoal);
        for( int i=0; i<maxcars; i++ )
        {
            if( places[i]!=null)
            {
                //System.out.println("places[i]!=null"+places[i].wType);
                minedCoal= places[i].load(WeightType.COAL, minedCoal);
                if( places[i].isFull() ){
                    LightCar car=places[i];

                    clearPlace(i);
                    Main.world.planning(car, Operations.CARRYCOAL);
                    accountOperated();
                }
            }
            if(minedCoal==0)break;
        }
    }

}
