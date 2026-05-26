package sample;

import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.shape.Rectangle;

import java.util.ArrayList;
import java.util.Random;

public class World {

    public static double mapwx=3000.0;
    public static double mapwy=2000.0;

    public static double mapscalewx=0.2;
    public static double mapscalewy=-0.2;

    //public static double mapwx=1000.0;
    //public static double mapwy=700.0;

    public static double carstepdelta = 30.0;

    private ArrayList<LightCar> cars = new ArrayList<LightCar>();
    private Random rnd = new Random();

    private boolean restMode=false;

    private Shahta shahta;
    private GOK gok;
    private Zavod zavod;
    private Baza baza;
    private Location idleLocation;

    private Location planovaijViddil[];
    private int nextPlan;

    public static int coalMined;
    public static int oreMined;
    public static int steelSmelted;
    public static int dollarsEarned;


    public World(Image imgshahta, Image imggok, Image imgzavod, Image imgbaza, Image imgidle )
    {
//        shahta= new Shahta(imgshahta,200.0,700.0);
//        gok = new GOK(imggok, 400.0, 50.0);
//        zavod = new Zavod(imgzavod, 1500.0, 500.0 );
//        baza = new Baza(imgbaza, 1000.0, 700.0 );

        shahta= new Shahta(imgshahta,400.0,1200.0);
        gok = new GOK(imggok, 100.0, 120.0);
        zavod = new Zavod(imgzavod, 2000.0, 100.0 );
        baza = new Baza(imgbaza, 1000.0, 1000.0 );


        planovaijViddil = new Location[]{shahta, gok, zavod, baza};
        nextPlan=0;

        idleLocation = Location.createIdleLocation(imgidle, 800.0, 700.0 );
    }

    public void planning(LightCar car, Operations op)
    {
        car.clearProcessing();
        switch(op)
        {
            case GETSOMEREST:
                idleLocation.aimCar(car);
                break;
            case CARRYCOAL:
                zavod.aimCar(car);
                break;
            case CARRYORE:
                zavod.aimCar(car);
                break;
            case CARRYSTEEL:
                baza.aimCar(car);
                break;

        }

    }

    public void unloadActive()
    {
        for (LightCar car : cars) {
            if( car.isActive() )
                car.unload();
        }
    }

    public void toggleRestMode()
    {
        if (!restMode) {

            for (LightCar car : cars) {

                if (car.isProcessing())
                {
                    freeStables(car);
                    car.clearProcessing();
                }
                car.clearAim();

                planning(car, Operations.GETSOMEREST);

            }
        }

        restMode=!restMode;

    }
    public boolean isCorrect( double x, double y)
    {

        if( shahta.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )return false;
        if( gok.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )return false;
        if( zavod.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )return false;
        if( baza.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )return false;

       for( LightCar car : cars )
           if( car.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )
               return false;

        return true;

    }

    public void generateCar(CarLevel cl )
    {
        LightCar c=new LightCar();

        double  x=0;
        double  y=0;

        do{
            x = (double) rnd.nextInt((int) mapwx);
            y = (double) rnd.nextInt((int) mapwy);
        }while( !isCorrect(x,y) );

        switch(cl){
            case LIGHT:
                 c= new LightCar( x, y );
                 break;
            case MEDIUM:
                  c= new MediumCar( x, y );
                   break;
            case HEAVY:
                c= new HeavyCar( x, y );
                break;
            }

        cars.add(c);

    }

    public void coalLoad(double x, double y)
    {
        for( LightCar car : cars )
            if( car.intersects(x,y,2.0, 2.0 ) )
            {
                if( car.load(WeightType.COAL,10)>0.0 ) {
                    car.unload();
                    car.load(WeightType.COAL,10);
                }
            }

    }

    public void oreLoad(double x, double y)
    {
        for( LightCar car : cars )
            if( car.intersects(x,y,2.0, 2.0 ) )
            {
                if( car.load(WeightType.ORE,10)>0.0 ) {
                    car.unload();
                    car.load(WeightType.ORE,10);
                }
            }

    }

    public void steelLoad(double x, double y)
    {
        for( LightCar car : cars )
            if( car.intersects(x,y,2.0, 2.0 ) )
            {
                if( car.load(WeightType.STEEL,10)>0.0 ) {
                    car.unload();
                    car.load(WeightType.STEEL,10);
                }
            }

    }

    public void activate(double x, double y) {

        for( LightCar car : cars )
            if( car.intersects(x,y,2.0, 2.0 ) )
            {
                if( !car.isActive() )car.activate();
                else
                    car.deactivate();

            }

    }

    public void carKeyMove(double dx, double dy)
    {
        for( LightCar car : cars )
            if( car.isActive() )
                car.keyMove(dx, dy);
    }

    public void outputActivatedCars()
    {   int count=0;

        for( LightCar car : cars ) {
            if (car.isActive())
                System.out.println(count+" "+car.toString());
            count++;
        }
    }

    public void whatToDo(LightCar car)
    {
/*        double  x=0;
        double  y=0;

        do{
            x = (double) rnd.nextInt((int) mapwx);
            y = (double) rnd.nextInt((int) mapwy);
        }while( !isCorrect(x,y) );


        car.setAim(x,y);
        System.out.println(x+" "+y);
  */
        if(restMode)return;

        if( shahta.worksWith(car) )return;
        if( gok.worksWith(car) )return;
        if( zavod.worksWith(car) )return;
        if( baza.worksWith(car) )return;

        if( car.carryWeight(WeightType.STEEL)) {
            zavod.aimCar(car);
            return;
        }

        if( planovaijViddil[nextPlan]==zavod )
        {
            if( car.carryWeight(WeightType.NONE) ) {
                if( zavod.getEmptyCars()>3 )
                {
                    nextPlan++;
                    if(nextPlan==planovaijViddil.length)nextPlan=0;
                }
            }
            else if( car.carryWeight(WeightType.COAL) )
            {
                if( zavod.getCoalCars()>3 ) {
                    idleLocation.aimCar(car);
                    return;
                }
            }
            else if( car.carryWeight(WeightType.ORE) )
            {
                if( zavod.getOreCars()>3 ) {
                    idleLocation.aimCar(car);
                    return;
                }
            }

        }


            planovaijViddil[nextPlan].aimCar(car);

            nextPlan++;
            if (nextPlan == planovaijViddil.length) nextPlan = 0;
    }

    public void freeStables(LightCar car)
    {
        if( shahta.freeStable(car) )return;
        if( gok.freeStable(car) )return;
        if( zavod.freeStable(car) )return;
        if( baza.freeStable(car) )return;
    }


    public static int mineCoal()
    {
        int coal = 20;
        coalMined+=coal;
//        System.out.println("COAL" + coalMined);
        return coal;
    }
    public static int mineOre()
    {
        int ore = 10;
        oreMined+=ore;
//        System.out.println("ORE" + oreMined);
        return ore;
    }

    public static void smeltSteel(int s)
    {
        steelSmelted+=s;
//        System.out.println("STEEL" + steelSmelted);
    }

    public static int smeltSteelLimit()
    {
        return 10;
    }

    public static void soldSteel(int s)
    {
        dollarsEarned+=s;
//        System.out.println("DOLLARS" + dollarsEarned);
    }

    public static int soldSteelLimit()
    {
        return 10;
    }
    public void lifeCycle()
    {
        shahta.lifeCycle();
        gok.lifeCycle();
        zavod.lifeCycle();
        baza.lifeCycle();

        for( LightCar car : cars )car.move();

    }

    public void installActivated()
    {
        for( LightCar car : cars )
        {
            if(car.isActive())
            {
                car.deactivate();

                double x= car.getX();
                double y= car.getY();

                if( shahta.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )
                {
                    shahta.aimCar(car);
                    return;
                }

                if( gok.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )
                {
                    gok.aimCar(car);
                    return;
                }

                if( zavod.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )
                {
                    zavod.aimCar(car);
                    return;
                }


                if( baza.intersects(x,y,LightCar.imgLightCar.getWidth(), LightCar.imgLightCar.getHeight()) )
                {
                    baza.aimCar(car);
                    return;
                }

            }
        }

    }
}
