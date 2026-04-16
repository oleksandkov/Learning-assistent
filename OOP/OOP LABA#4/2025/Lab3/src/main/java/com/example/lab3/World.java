package com.example.lab3;

import javafx.application.Platform;
import javafx.geometry.Bounds;
import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.shape.Rectangle;

import java.util.ArrayList;
import java.util.Random;

public class World {
    public static double mapwx=3000.0;
    public static double mapwy=2000.0;


    public static Image imgrabbit;
    public static Image imgwolf;
    public static Image imgcarrot;
    public static Image imgcornField;
    public static Image imgRabbitHole;
    public static Image imgGarden;

    public static Random rand;
    static{
        rand = new Random(/*System.currentTimeMillis()*/);
    }
    public static Group group;
    public static final int numCarrots = 7;
    private HelloApplication window;

    private ArrayList<Rabbit> herd = new ArrayList<>();
    private Wolf wolf;
    private ArrayList<Carrot> carrots = new ArrayList<>();

    RabbitHole rabbitHole;
    CornField cornField;
    Garden garden;
    private static World world;
    public World( HelloApplication helloApp, Group group, Image imgrabbit, Image imgwolf,
                  Image imgcarrot, Image imgcornField, Image imgRabbitHole, Image imgGarden ){

        world= this;
        window= helloApp;

        World.group=group;
        World.imgrabbit =  imgrabbit;
        World.imgcarrot = imgcarrot;
        World.imgcornField = imgcornField;
        World.imgRabbitHole = imgRabbitHole;
        World.imgGarden = imgGarden;
        World.imgwolf = imgwolf;

        rabbitHole = new RabbitHole(imgRabbitHole,50, 100);

        cornField = new CornField(imgcornField, 1700, 350);

        garden = new Garden(imgGarden, 500, 600 );

        createNewRabbit("Obama", "70",  "500", "100" );
        createNewRabbit("Trump", "90",  "100", "500" );
        createNewRabbit("Reagan","50",  "500", "500" );
        createNewRabbit("Biden", "60",  "300", "300" );

        wolf= new Wolf("Hover", 100, 750, 400);
        wolf.resurrect();
    }

    public void initialize(){
        for( int i=0; i<numCarrots; ++i ){
            carrots.add( new Carrot( getScreenWX(), getScreenWY()) );
        }
    }

    public double getScreenWX(){
        return window.getScreenWX();
    }

    public double getScreenWY(){
        return window.getScreenWY();
    }
    public ArrayList<String> getParamsToChange( int index ){
        Rabbit r= herd.get(index);

        ArrayList<String> arr= new ArrayList<String>();
        arr.add( r.getName() );
        arr.add( r.getHealthStr() );
        arr.add( r.getXStr() );
        arr.add( r.getYStr() );
        return arr;
    }

    public ArrayList<String> getNames(){
        ArrayList<String> arr = new ArrayList<>();

        for( Rabbit r:herd ){
            arr.add(r.toString() );
        }

        return arr;
    }

    public void changeRabbit(int rabbitIndex, String sName, String sHealth, String sX, String sY ){
        Rabbit r= herd.get(rabbitIndex);

        r.setName(sName);
        r.setHealth(sHealth);
        r.setX(sX);
        r.setY(sY);
    }

    public void createNewRabbit(String sName, String sHealth, String sX, String sY ){

       // System.out.printf("sName=%s sHealth=%s sX=%s sY=%s \n", sName, sHealth, sX, sY );

        if( sName.equals("") ) sName="Rabbit";

        double h;
        try {
            h= Double.parseDouble(sHealth);
        }
        catch(Exception e){
            h=0.0;
        }

        double x;
        try {
            x= Double.parseDouble(sX);
        }
        catch(Exception e){
            x=0.0;
        }


        double y;
        try {
            y= Double.parseDouble(sY);
        }
        catch(Exception e){
            y=0.0;
        }

        Rabbit newRabbit = new Rabbit(sName,h,x,y);
        newRabbit.resurrect();
        newRabbit.flipActivation();
        herd.add( newRabbit );
    }


    public static void askWorldfreeStables(Rabbit rbt){
        world.freeStables(rbt);
    }
    public void freeStables(Rabbit rbt){
        if( rabbitHole.freeStable(rbt) )return;

        if( cornField.freeStable(rbt) )return;
    }

    public static void askWorldwhatToDo( Wolf wolf ){
        world.whatToDo(wolf);
    }
    public static void askWorldwhatToDo(Rabbit rbt){
        world.whatToDo(rbt);
    }

    public void planning(Rabbit rbt, Operations op)
    {
        rbt.clearProcessing();
        switch(op)
        {
            case FINDFOOD:
                cornField.aimCar(rbt);
                break;
            case SEEKSHELTER:
                rabbitHole.aimCar(rbt);
                break;
        }

    }

    public static void askWorldplanning(Rabbit rbt, Operations op) {

        world.planning( rbt, op);
    }
    public static void planningSwitch(Rabbit rbt, Location loc){
        if( loc instanceof RabbitHole )world.planning(rbt, Operations.FINDFOOD);

        world.planning(rbt, Operations.SEEKSHELTER);
    }
    public void whatToDo(Wolf wolf){

        double x;
        double y;

        if( World.rand.nextDouble() < 0.3 ){
            x = cornField.getRandomX();
            y = cornField.getRandomY();
        }
        else {

            x = World.rand.nextDouble() * World.mapwx;
            y = World.rand.nextDouble() * World.mapwy;
        }
        wolf.setAim(x,y);

    }
    public void whatToDo(Rabbit rbt){

        if( rabbitHole.worksWith(rbt) )return;
        if( cornField.worksWith(rbt) )return;



        if( rbt.isHungry() ){
            cornField.aimCar(rbt);
            return;
        }

        rabbitHole.aimCar(rbt);
    }
    public void deleteActive(){
        ArrayList<Rabbit> tmp = new ArrayList<>();

        for( Rabbit r:herd ){
            if( r.isActive() ){
                r.harakiri();
            }
            else{
                tmp.add(r);
            }

        }
        herd= tmp;

    }

    public void rabbitEatsCarrots( Rabbit r ){
        for( int i=0; i<carrots.size(); ++i ){
            if( r.getBoundsInParent().intersects( carrots.get(i).getBoundsInParent() )  ){
                r.eat( carrots.get(i).getFood() );
                carrots.get(i).harakiri();
                carrots.remove(i);
                return;
            }
        }
    }
    public void moveActive( double dx, double dy){
        for( Rabbit r:herd ){
            if( r.isActive() ){
                r.moveKey(dx, dy);
                //------------------------------
                rabbitEatsCarrots( r );
            }
        }
    }

    public static final int stepMax=3;

    public Rectangle findClonePosition(double x, double y, double wx, double wy ){
        x=x+Rabbit.imageDeltaX();
        y=y+Rabbit.imageDeltaY();

        //System.out.println("findClonePosition="+x+" "+y+" "+wx+" "+wy);

        Rectangle r = new Rectangle();
        r.setX(x);
        r.setY(y);
        r.setWidth(wx);
        r.setHeight(wy);

        double [][] deltas = new double[][]{ {1.0,0.0},{1.0, 1.0},{0.0, 1.0},{-1.0, 1.0},
                                             {-1.0, 0.0},{-1.0, -1.0},{0.0, -1.0},{1.0, -1.0} };

        for( int i=1; i<=stepMax; ++i ){
            for( int j=0; j<8; ++j ){
                double newx = x + wx*1.1*deltas[j][0]*i;
                double newy = y + wy*1.1*deltas[j][1]*i;


                //System.out.println("for( int j=0; j<8; ++j )="+j);
                //System.out.println("double newx = x + wx*1.1*deltas[j][0];="+newx+" "+x+" "+wx+" "+deltas[j][0]);

                if( newx<0.0 )continue;
                if( newy<0.0 )continue;
                if( !( (newx+wx)<getScreenWX()) )continue;
                if( !( (newy+wy)<getScreenWY()) )continue;

                r.setX(newx);
                r.setY(newy);


                boolean found=false;
                //int count=0;
                for (Rabbit rabbit : herd) {

                    //System.out.println("for (Rabbit rabbit : herd)"+count);
                    //count++;


                    //System.out.println("rabbit.getBoundsInParent()="+rabbit.getBoundsInParent());
                    //System.out.println("r.getBoundsInParent()"+r.getBoundsInParent());
                    if( rabbit.getBoundsInParent().intersects( r.getBoundsInParent() )  ){
                        found=true;
                        break;
                    }

                }

                if( !found ){
                    r.setX(newx-Rabbit.imageDeltaX());
                    r.setY(newy-Rabbit.imageDeltaY());

                    //System.out.println("Result of findClonePosition="+r);
                    return r;

                }

            }
        }

        return null;
    }

//    public Rectangle findClonePosition(double x, double y, double wx, double wy )

        public void copyActive() throws CloneNotSupportedException{

        ArrayList<Rabbit> tmp = new ArrayList<>();

        for (Rabbit r : herd) {
            if (r.isActive()) {

                Rectangle rect = findClonePosition( r.getX(), r.getY(),
                        r.getImageWX(), r.getImageWY() );

                //System.out.println("findClonePosition="+rect);

                if( rect!=null ) {
                    Rabbit newRabbit = (Rabbit)r.clone();
                    newRabbit.setX(rect.getX());
                    newRabbit.setY(rect.getY());
                    //System.out.println("Created rabbit="+newRabbit);
                    newRabbit.resurrect();
                    tmp.add(newRabbit);
                }
            }
            tmp.add(r);
        }

        herd=tmp;
    }

    public void installActivated() {
        for( Rabbit rbt : herd ) {
            if( !rbt.isActive() )continue;

            if(rbt.isProcessed() ){
                rbt.clearProcessing();
                World.askWorldfreeStables(rbt);

            }

            rbt.clearAim();

            rbt.flipActivation();

            if(rabbitHole.intersects(rbt.getBoundsInParent())){
                rabbitHole.aimCar(rbt);
                return;
            }

            if( cornField.intersects(rbt.getBoundsInParent())){
                cornField.aimCar(rbt);
                return;
            }
            rbt.flipActivation();

        }
    }

    public boolean mousePrimaryActivate(double mx, double my){
        boolean flg=false;
        for( Rabbit r:herd ){
            if( r.mousePrimaryActivate(mx, my ) )flg=true;
        }
        return flg;
    }
    public void escapeActivate(){
        for( Rabbit r:herd ){
            r.cancelActivation();
        }
    }
    public void searchByName(String sName ){

        for( Rabbit r:herd )
            if( r.getName().equals(sName) ){
                System.out.print("Заєць " + sName + " знаходиться в координатах "
                        +r.getXStr() + " " + r.getYStr() + " (" );

                if( rabbitHole.locatedIn(r) ){
                    System.out.println("in RabbitHole)");
                    continue;
                }

                if( cornField.locatedIn(r) ){
                    System.out.println("in CornField");
                    continue;
                }

                System.out.println(" run loose ");
            }

    }
    public void lifeCycle(){
        ArrayList<Rabbit> tmp = new ArrayList<>();

        rabbitHole.lifeCycle();
        cornField.lifeCycle();

        for( Rabbit r:herd ){
            if( r.physiology() )tmp.add(r);
            else{
                if(!r.isDead() )tmp.add(r);
                else{
                    r.harakiri();
                    continue;
                }
            }

            r.autoMove();
            rabbitEatsCarrots( r );
        }
//            if( r.isEmptyAim() ){
//                double newAimX = World.rand.nextDouble()*getScreenWX();
//                double newAimY = World.rand.nextDouble()*getScreenWY();
//
//                r.setAim(newAimX,newAimY);
//            }


        if( wolf!=null ){
            wolf.autoMove();
        }

        herd=tmp;
    }
    public static boolean wolfRestrictionsSatisfied(Bounds bounds ){
        return world.wolfRestrictionsSatisfiedNonStatic( bounds);
    }
    public boolean wolfRestrictionsSatisfiedNonStatic(Bounds bounds ){
        if( bounds.intersects( garden.getBoundsInParent() )  )return false;

        if( bounds.intersects( rabbitHole.getBoundsInParent() )  )return false;

        return true;
    }
    public void moveKey(String sX, String sY ){
        boolean error=false;
        double dx;
        try {
            dx= Double.parseDouble(sX);
        }
        catch(Exception e){
            dx=0.0;
            error=true;
        }

        double dy;
        try {
            dy= Double.parseDouble(sY);
        }
        catch(Exception e){
            dy=0.0;
            error=true;
        }

        if( !error )moveActive(dx, dy);


    }
       private void ensureFXThread(Runnable action) {
        if (Platform.isFxApplicationThread()) {
            action.run();
        } else {
            Platform.runLater(action);
        }
    }

    public void CLIescape(){
        ensureFXThread( () -> escapeActivate());
    }

    public void CLImove( String sX, String sY ){
        ensureFXThread( () -> moveKey(sX, sY) );
    }

    public void CLIexit(){
        ensureFXThread( () ->Platform.exit() );
    }

    public void CLIcreate( String sName, String sHealth, String sX, String sY ){
        ensureFXThread( () -> createNewRabbit(sName,sHealth,sX,sY) );
    }

    public void CLIsearch( String sName ){
        ensureFXThread( () -> searchByName(sName) );
    }

    public void CLIinitialize(){
        ensureFXThread( () -> initialize() );
    }
}
