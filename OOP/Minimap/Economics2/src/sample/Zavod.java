package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Zavod extends Location {

    public boolean intersects( double x, double y, double wx, double wy ) {
        if( image.intersects(x,y,wx,wy) )return true;

        return false;
    }


    public Zavod(Image img, double x, double y)
    {
        super(-105.0, -55, 0, 55, 15, 4, 0);

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


//---------------------------------------------------
        time_delay=40;
        time_current=0;

        postInitialization();
    }

    public int searchCoal() {
        for (int i = 0; i < maxcars; i++) {
             if(places[i]!=null)
             {
                 if(places[i].getWeightType()==WeightType.COAL) return i;
             }
        }

        return -1;
    }

    public int searchOre() {
        for (int i = 0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if(places[i].getWeightType()==WeightType.ORE ) return i;
            }
        }

        return -1;
    }

    public int searchSteel() {
        for (int i = 0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if( (places[i].getWeightType()==WeightType.STEEL) ||
                        (places[i].getWeightType()==WeightType.NONE) ) return i;
            }
        }

        return -1;
    }

    public int getEmptyCars()
    {
        int count=0;
        for (int i = 0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if( places[i].getWeightType()==WeightType.NONE ) count++;
            }
        }
        return count;
    }

    public void lifeCycle() {
        time_current++;
        if (time_current < time_delay) return;
        time_current = 0;

        int icoal= searchCoal();   //индекс угольного грузовика
        int iore= searchOre();     //индекс грузовика с рудой
        int isteel= searchSteel(); //индекс грузовика для погрузки стали

        if( (icoal==-1) || (iore==-1) || (isteel==-1) )return;

        int smeltedSteel = ( (places[icoal].getMass()/2) < places[iore].getMass() )?
                               (places[icoal].getMass()/2) :  places[iore].getMass();

        smeltedSteel = ( smeltedSteel<World.smeltSteelLimit() )?
                           smeltedSteel:World.smeltSteelLimit() ;

        smeltedSteel = ( smeltedSteel < places[isteel].getFree() )?
                smeltedSteel:places[isteel].getFree();

        World.smeltSteel(smeltedSteel);

        places[icoal].unload(WeightType.COAL, smeltedSteel*2);
        places[iore].unload(WeightType.ORE, smeltedSteel);

        int rez= places[isteel].load(WeightType.STEEL, smeltedSteel);
        //System.out.println("places[isteel].load(WeightType.STEEL, smeltedSteel); "+isteel + " " + smeltedSteel + "rez" + rez);

        if( places[isteel].isFull() )
        {
            LightCar car=places[isteel];
            clearPlace(isteel);

            Main.world.planning(car, Operations.CARRYSTEEL);
            accountOperated();

        }

        if( places[icoal].getMass() < 10 )
        {
            places[icoal].unload();
            LightCar car = places[icoal];
            clearPlace(icoal);

            Main.world.planning(car, Operations.GETSOMEREST);
        }

        if( places[iore].getMass() < 10 )
        {
            places[iore].unload();
            LightCar car = places[iore];
            clearPlace(iore);

            Main.world.planning(car, Operations.GETSOMEREST);
        }

        for(int i=0; i < maxcars; i++) {
            if(places[i]!=null)
            {
                if( places[i].getWeightType()==WeightType.NONE )
                    if( !places[i].carryWeight(WeightType.STEEL))
                    {
                        LightCar car = places[i];
                        clearPlace(i);
                        Main.world.planning(car, Operations.GETSOMEREST);
                    }
            }
        }

    }
}
