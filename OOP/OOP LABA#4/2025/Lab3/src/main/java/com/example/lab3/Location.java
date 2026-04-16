package com.example.lab3;
import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.text.Font;

public class Location {
    protected double rbtx, rbty, rbtdx, rbtdy, rbtNCols;
    protected int maxRabbits;
    protected int current_rbt;
    protected Rabbit [] places;
    protected ImageView image;

    protected Label rbtsOperated;
    protected Label rbtsProcessing;

    protected int irbtsOperated;
    protected int irbtsProcessing;

    protected int time_delay;
    protected int time_current;

    public boolean intersects( Bounds bounds ) {
        if( image.intersects(bounds) )return true;

        return false;
    }

    public Bounds getBoundsInParent(){
        return image.getBoundsInParent();
    }
    public boolean coincides (double ax, double ay, double bx, double by )
    {
        if(  (Math.abs(ax-bx)+ Math.abs(ay-by)) < 1.0 )return true;

        return false;
    }
    public double getXPosition(int i){
        if( i<0 )return -1000.0;
        if( !(i<maxRabbits) )return -1000.0;

        double x= image.getX();
        double y= image.getY();

        y= y + rbty;

        do{

            if( i<rbtNCols )return x+rbtx+i*rbtdx;

            y=y + rbtdy;
            i-=rbtNCols;

        }while(true);

    }

    public double getRandomX(){
        return image.getX() + image.getFitWidth()*World.rand.nextDouble();
    }

    public double getRandomY(){
        return image.getY() + image.getFitHeight()*World.rand.nextDouble();
    }

    public double getYPosition(int i){
        if( i<0 )return -1000.0;
        if( !(i<maxRabbits) )return -1000.0;

        double x= image.getX();
        double y= image.getY();

        y= y + rbty;

        do{

            if( i<rbtNCols )return y;

            y=y + rbtdy;
            i-=rbtNCols;

        }while(true);

    }

    public boolean locatedIn( Rabbit rbt ){
        for( int i=0; i<maxRabbits; i++ ) {
            if( places[i] == rbt ) return true;
        }

        return false;
    }
    public boolean worksWith(Rabbit rbt)
    {
        double mashinax= rbt.getX();
        double mashinay= rbt.getY();

        for( int i=0; i<maxRabbits; i++ )
        {
            if( coincides( mashinax, mashinay,
                    getXPosition(i), getYPosition(i) ) )
            {
                //System.out.println("coincides!");
                if( places[i]!=null )  World.planningSwitch(rbt, this);
                else
                {
                    places[i]=rbt;
                    rbt.setProcessing();
                    accountPlaces();
                }
                return true;
            }
        }

        return false;
    }


    public boolean freeStable(Rabbit rbt)
    {
        for( int i=0; i<maxRabbits; i++ ) {
            if( places[i]==rbt)
            {
                clearPlace(i);
                return true;
            }
        }
        return false;
    }

    public void aimCar(Rabbit rbt)
    {
        rbt.setAim(  getXPosition(current_rbt), getYPosition(current_rbt) );
        current_rbt++;
        if(current_rbt==maxRabbits)current_rbt=0;
    }

    public void postInitialization()
    {
        double x= image.getX();
        double y= image.getY();

        rbtsOperated=new Label();
        rbtsOperated.setFont(new Font(24));
        rbtsOperated.setLayoutX(x+0.0);
        rbtsOperated.setLayoutY(y-30.0);

        rbtsProcessing=new Label();
        rbtsProcessing.setFont(new Font(24));
        rbtsProcessing.setLayoutX(x+150.0);
        rbtsProcessing.setLayoutY(y-30.0);

        World.group.getChildren().addAll(rbtsOperated,rbtsProcessing);
    }

    public void accountOperated()
    {
        irbtsOperated++;
        rbtsOperated.setText("Complete:"+Integer.toString(irbtsOperated));
    }

    public void clearPlace(int i)
    {
        places[i]=null;
        irbtsProcessing--;
        rbtsProcessing.setText("Processing:"+Integer.toString(irbtsProcessing));
    }

    public void accountPlaces()
    {
        irbtsProcessing++;
        rbtsProcessing.setText("Processing:"+Integer.toString(irbtsProcessing));
    }

    public Location( double rbtx, double rbty, double rbtdx, double rbtdy,
                     int maxRabbits, int rbtNCols)
    {
        this.rbtx=rbtx;
        this.rbty=rbty;
        this.rbtdx=rbtdx;
        this.rbtdy=rbtdy;
        this.rbtNCols=rbtNCols;

        this.maxRabbits=maxRabbits;
        this.current_rbt=0;

        places=new Rabbit[this.maxRabbits];

        time_delay=10;
        time_current=0;

        irbtsOperated=0;
        irbtsProcessing=0;

    }

    public String getName(){
        return "Location";
    }
}
