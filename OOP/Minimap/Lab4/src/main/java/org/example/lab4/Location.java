package org.example.lab4;


import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.text.Font;

import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;

public class Location {
    public void load(BufferedReader bufferedReader) throws IOException {
        String text;

        text = bufferedReader.readLine();
        int cstd= Integer.parseInt(text);
        current_std= cstd;

        text = bufferedReader.readLine();
        int hmany= Integer.parseInt(text);
        places=new Student[hmany];

        for( int i=0; i<hmany; ++i ){
            text = bufferedReader.readLine();
            int idx= Integer.parseInt(text);

            if( idx<0 )places[i]=null;
            else places[i]= World.askWorldStudAddress(idx);

        }

        text = bufferedReader.readLine();
        int i1= Integer.parseInt(text);
        istdsProcessing= i1;

        text = bufferedReader.readLine();
        int i2= Integer.parseInt(text);
        time_delay= i2;

        text = bufferedReader.readLine();
        int i3= Integer.parseInt(text);
        time_current= i3;

        text = bufferedReader.readLine();
        setString(text);

    }

    public void save(FileWriter fileWriter ) throws IOException {
        fileWriter.write( Integer.toString(current_std)+"\n");
        fileWriter.write( Integer.toString(places.length)+"\n");

        for( int i=0; i<places.length; ++i ){
            if( places[i]==null )        fileWriter.write( Integer.toString(-1)+"\n");
            else {
                int rez= World.askWorldStudNumber(places[i]);
                fileWriter.write( Integer.toString(rez)+"\n");

            }
        }

        fileWriter.write( Integer.toString(istdsProcessing)+"\n");
        fileWriter.write( Integer.toString(time_delay)+"\n");
        fileWriter.write( Integer.toString(time_current)+"\n");

        fileWriter.write( getString()+"\n");
    }

    protected double stdx, stdy, stddx, stddy, stdNcols;
    protected int maxStd;
    protected int current_std;

    protected Student [] places;
    protected ImageView image;

    protected Label stdsString;
    protected Label stdsProcessing;

    protected int istdsProcessing;

    protected int time_delay;
    protected int time_current;

    public boolean intersects( Bounds bounds ) {
        if( image.intersects(bounds) )return true;

        return false;
    }

    public Bounds getBoundsInParent(){
        return image.getBoundsInParent();
    }

    public boolean coincides (double ax, double ay, double bx, double by ) {
        if(  (Math.abs(ax-bx)+ Math.abs(ay-by)) < 1.0 )return true;

        return false;
    }

    public double getXPosition(int i){
        if( i<0 )return -1000.0;
        if( !(i<maxStd) )return -1000.0;

        double x= image.getX();
        double y= image.getY();

        y= y + stdy;

        do{

            if( i<stdNcols )return x+stdx+i*stddx;

            y=y + stddy;
            i-=stdNcols;

        }while(true);

    }

    public double getYPosition(int i){
        if( i<0 )return -1000.0;
        if( !(i<maxStd) )return -1000.0;

        double x= image.getX();
        double y= image.getY();

        y= y + stdy;

        do{

            if( i<stdNcols )return y;

            y=y + stddy;
            i-=stdNcols;

        }while(true);

    }

    public boolean locatedIn( Student st ){
        for( int i=0; i<maxStd; i++ ) {
            if( places[i] == st ) return true;
        }

        return false;
    }

    public boolean worksWith(Student st)
    {
        double mashinax= st.getX();
        double mashinay= st.getY();

        for( int i=0; i<maxStd; ++i )
        {
            if( coincides( mashinax, mashinay,
                    getXPosition(i), getYPosition(i) ) )
            {
                //System.out.println("coincides!");
                if( places[i]!=null )  World.planningSwitch(st, this);
                else
                {
                    places[i]=st;
                    st.setProcessing();
                    st.startInternalTimer();
                    accountPlaces();
                }
                return true;
            }
        }

        return false;
    }

    public boolean freePlaces(Student st)
    {
        for( int i=0; i<maxStd; i++ ) {
            if( places[i]==st)
            {
                clearPlace(i);
                return true;
            }
        }
        return false;
    }

    public void aimCar(Student st)
    {
        st.setAim(  getXPosition(current_std), getYPosition(current_std) );
        current_std++;
        if(current_std==maxStd)current_std=0;
    }

    public void postInitialization()
    {
        double x= image.getX();
        double y= image.getY();

        stdsString=new Label();
        stdsString.setFont(new Font(24));
        stdsString.setLayoutX(x+0.0);
        stdsString.setLayoutY(y-30.0);
        stdsString.setText(" ");

        stdsProcessing=new Label();
        stdsProcessing.setFont(new Font(24));
        stdsProcessing.setLayoutX(x+150.0);
        stdsProcessing.setLayoutY(y-30.0);
        stdsProcessing.setText("Processing:");

        HelloApplication.group.getChildren().addAll(stdsString, stdsProcessing);
    }

    public void setString(String s) {
        stdsString.setText(s);
    }

    public String getString(){
        return stdsString.getText();
    }

    public void clearPlace(int i) {
        if( places[i]!=null ) {
            places[i].clearProcessing();
            places[i] = null;
            istdsProcessing--;
            stdsProcessing.setText("Processing:" + Integer.toString(istdsProcessing));
        }
    }

    public void accountPlaces()
    {
        istdsProcessing++;
        stdsProcessing.setText("Processing:"+Integer.toString(istdsProcessing));
    }

    public Location( double rbtx, double rbty, double rbtdx, double rbtdy,
                     int maxRabbits, int rbtNCols)
    {
        this.stdx=rbtx;
        this.stdy=rbty;
        this.stddx=rbtdx;
        this.stddy=rbtdy;
        this.stdNcols=rbtNCols;

        this.maxStd=maxRabbits;
        this.current_std=0;

        places=new Student[this.maxStd];

        time_delay=10;
        time_current=0;

        istdsProcessing=0;

    }

    public String getName(){
        return "Location";
    }

    public double studyPower(){
        return 0.0;
    }
}
