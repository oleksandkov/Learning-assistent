package sample;

import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.text.Font;

public class Location {
    protected double carx, cary, cardx, cardy;
    protected int maxcars;
    protected int current_car;
    protected int caroperation;
    protected LightCar [] places;
    protected ImageView image;
    protected ImageView miniImage;

    protected Label carsOperated;
    protected Label carsProcessing;

    protected int icarsOperated;
    protected int icarsProcessing;

    protected int time_delay;
    protected int time_current;

    public static Location createIdleLocation(Image img, double x, double y )
    {
        Location location = new Location(50.0,0.0,105.0, 0.0, 20,1,0);
        location.image= new ImageView(img);

        Main.group.getChildren().add(location.image);

        location.image.setX(x);
        location.image.setY(y);

        location.miniImage= new ImageView(img);
        location.miniImage.setPreserveRatio(true);
        location.miniImage.setFitHeight(60.0);
        Main.miniGroup.getChildren().add(location.miniImage);

        location.miniImage.setX(x*Main.SCALE);
        location.miniImage.setY(y*Main.SCALE);


        return location;
    }

    public boolean coincides (double ax, double ay, double bx, double by )
    {
        if(  (Math.abs(ax-bx)+ Math.abs(ay-by)) < 1.0 )return true;

        return false;
    }

    public boolean worksWith(LightCar car)
    {
        double mashinax= car.getX();
        double mashinay= car.getY();

        double x= image.getX();
        double y= image.getY();

        for( int i=0; i<maxcars; i++ )
        {
            if( coincides( mashinax, mashinay,
                           x+carx+i*cardx, y+cary+i*cardy ) )
            {
                //System.out.println("coincides!");
                if( places[i]!=null )Main.world.planning(car, Operations.GETSOMEREST);
                else
                {
                    places[i]=car;
                    car.setProcessing();
                    accountPlaces();
                }
                return true;
            }
        }

        return false;
    }

    public int getCoalCars()
    {
        int count = 0;

        for( int i=0; i<maxcars; i++ ) {
            if( places[i]!=null)
            {
                if(places[i].getWeightType()==WeightType.COAL)
                    count++;

            }
        }

        return count;
    }

    public int getOreCars()
    {
        int count = 0;

        for( int i=0; i<maxcars; i++ ) {
            if( places[i]!=null)
            {
                if(places[i].getWeightType()==WeightType.ORE)
                    count++;

            }
        }

        return count;
    }

    public boolean freeStable(LightCar car)
    {
        for( int i=0; i<maxcars; i++ ) {
            if( places[i]==car)
            {
                clearPlace(i);
                return true;
            }
        }
        return false;
    }

    public void aimCar(LightCar car)
    {
        car.setAim(image.getX()+carx+current_car*cardx,
                    image.getY()+cary+current_car*cardy );

        current_car++;
        if(current_car==maxcars)current_car=0;
    }

    public void postInitialization()
    {
        double x= image.getX();
        double y= image.getY();

        carsOperated=new Label();
        carsOperated.setFont(new Font(24));
        carsOperated.setLayoutX(x+0.0);
        carsOperated.setLayoutY(y-30.0);

        carsProcessing=new Label();
        carsProcessing.setFont(new Font(24));
        carsProcessing.setLayoutX(x+150.0);
        carsProcessing.setLayoutY(y-30.0);

        Main.group.getChildren().addAll(carsOperated,carsProcessing);
    }

    public void accountOperated()
    {
        icarsOperated++;
        carsOperated.setText("Complete:"+Integer.toString(icarsOperated));
    }

    public void clearPlace(int i)
    {
        places[i]=null;
        icarsProcessing--;
        carsProcessing.setText("Processing:"+Integer.toString(icarsProcessing));
    }

    public void accountPlaces()
    {
        icarsProcessing++;
        carsProcessing.setText("Processing:"+Integer.toString(icarsProcessing));
    }

    public Location( double carx, double cary, double cardx, double cardy,
                     int maxcars, int caroperation, int current_car )
    {
        this.carx=carx;
        this.cary=cary;
        this.cardx=cardx;
        this.cardy=cardy;

        this.maxcars=maxcars;
        this.caroperation=caroperation;
        this.current_car=current_car;

        places=new LightCar[maxcars];

        time_delay=10;
        time_current=0;

        icarsOperated=0;
        icarsProcessing=0;


    }

}
