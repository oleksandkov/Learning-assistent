package sample;

import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;

public class LightCar {

    public static double wx=0.0;
    public static double wy=-22.0;
    public static double tx= 50.0;
    public static double ty= -22.0;
    public static double activerectwx= 110.0;
    public static double activerectwy= 79.0;
    public static double activerectdx=  -5.0;
    public static double activerectdy= -27.0;

    public static Image imgLightCar;
    public static Image imgNone;
    public static Image imgCoal;
    public static Image imgOre;
    public static Image imgSteel;

    protected ImageView imgv;
    protected ImageView miniImgv;

    protected static boolean once=false;
    protected ImageView imgw;

    protected Rectangle rectActive;
    protected boolean isActive;

    protected boolean isProcessing;

    protected WeightType wType;
    protected int mass;

    private double aimx;
    private double aimy;

    protected Label mtext;

    public void clearAim()
    {
        aimx = aimy = -1000.0;
    }

    public WeightType getWeightType()
    {
        return wType;
    }

    public int getMass()
    {
        return mass;
    }

    public double getAimX(){
        return aimx;
    }

    public double getAimY(){
        return aimy;
    }

    public boolean isEmptyAim()
    {
        if( (aimx<0) || (aimy<0) )return true;

        return false;
    }

    public boolean isProcessing()
    {
        return isProcessing;
    }

    @Override
    public String toString() {
        String rez;
        rez= stringCarType() + " " + mass + " " +  wType + " isProcessing:"+isProcessing+
                " aimx"+aimx +" aimy"+aimy+ ", mtext=" + mtext;

        return rez;
    }

    public String stringCarType()
    {
        return "LightCar";
    }
    public void setProcessing()
    {
        isProcessing=true;
    }

    public void clearProcessing()
    {
        isProcessing=false;
    }

    public void init()
    {
        double x= imgv.getX();
        double y= imgv.getY();

        mass= 0;
        wType=WeightType.NONE;
        imgw= new ImageView(imgNone);
        Main.group.getChildren().add(imgw);

        imgw.setX(x+wx);
        imgw.setY(y+wy);

        mtext=new Label("TEXT");
        mtext.setFont(new Font(16));
        Main.group.getChildren().add(mtext);
        mtext.setText("0");

        mtext.setLayoutX(x+tx);
        mtext.setLayoutY(y+ty);
        //----------------------------------------
        rectActive=new Rectangle(activerectwx,activerectwy);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.TRANSPARENT);

        Main.group.getChildren().add(rectActive);
        rectActive.setX(x+activerectdx);
        rectActive.setY(y+activerectdy);

        isActive=false;

        isProcessing=false;

        clearAim();

    }

    public LightCar( double x, double y) {

        imgv = new ImageView(imgLightCar);
        Main.group.getChildren().add(imgv);

        imgv.setX(x);
        imgv.setY(y);

//        if( !once ) {
//            once=true;
//        }
            miniImgv = new ImageView(imgLightCar);
            miniImgv.setPreserveRatio(true);
            miniImgv.setFitHeight(10.0);
            Main.miniGroup.getChildren().add(miniImgv);

            miniImgv.setX(x*Main.SCALE);
            miniImgv.setY(y*Main.SCALE);



        init();

    }
    public LightCar()
    {

    }

    public boolean isActive()
    {
        return isActive;
    }
    public void activate()
    {
        isActive=true;

        rectActive.setStroke(Color.YELLOW);

    }

    public void deactivate()
    {
        isActive=false;

        rectActive.setStroke(Color.TRANSPARENT);

    }

    public double speedX()
    {
        return 4.0;
    }
    public double speedY()
    {
        return 4.0;
    }

    public void setAim(double x,double y)
    {
        aimx=x;
        aimy=y;
    }

    public double getX()
    {
        return imgv.getX();
    }

    public double getY()
    {
        return imgv.getY();
    }

    public void keyMove(double dx, double dy)
    {
        if( isProcessing )
        {
            Main.world.freeStables(this);
            clearProcessing();
        }

        clearAim();

        double x= imgv.getX() + dx;
        double y= imgv.getY() + dy;

        imgv.setX(x);
        imgv.setY(y);

        imgw.setX(x + wx);
        imgw.setY(y + wy);

        mtext.setLayoutX(x + tx);
        mtext.setLayoutY(y + ty);

        rectActive.setX(x + activerectdx);
        rectActive.setY(y + activerectdy);


    }

    public void move()
    {
        if( isActive )return;

        if( isProcessing )return;

        if( isEmptyAim() ) {
            Main.world.whatToDo(this);
        }
        else {

            double x = imgv.getX();
            double y = imgv.getY();

            if ((Math.abs(x - aimx) + Math.abs(y - aimy)) < 1.0) {
                clearAim();
            } else {

                double signdx = Math.signum(aimx - x);
                double dx = Math.abs(aimx - x);
                dx = ((dx < speedX()) ? dx : speedX());
                dx = signdx * dx;

                double signdy = Math.signum(aimy - y);
                double dy = Math.abs(aimy - y);
                dy = ((dy < speedY()) ? dy : speedY());
                dy = signdy * dy;
                //------------------------------------------

                x += dx;
                y += dy;

                imgv.setX(x);
                imgv.setY(y);

                imgw.setX(x + wx);
                imgw.setY(y + wy);

                mtext.setLayoutX(x + tx);
                mtext.setLayoutY(y + ty);

                rectActive.setX(x + activerectdx);
                rectActive.setY(y + activerectdy);

                miniImgv.setX(x*Main.SCALE);
                miniImgv.setY(y*Main.SCALE);
            }

        }
    }

    public boolean isFull()
    {
        if( mass==massSize() )return true;

        return false;
    }

    public int getFree()
    {
        return (massSize()-mass) ;
    }

    public int massSize()
    {
        return 50;
    }

    public boolean carryWeight( WeightType wt )
    {
        if( wt == WeightType.COAL )return true;

        return false;
    }

    public int unload()
    {
        int w = mass;
        unload( wType, mass );
        return w;
    }

    public int unload(WeightType wt, int m)
    {
        if( wType!=wt )return 0;

        if( wType==WeightType.NONE )return 0;

        int w = (mass<m)?mass:m;

        mass-=w;

        if(mass==0)
        {
            Main.group.getChildren().remove(imgw);

            imgw= new ImageView(imgNone);

            Main.group.getChildren().add(imgw);

            double x= imgv.getX();
            double y= imgv.getY();

            imgw.setX(x+wx);
            imgw.setY(y+wy);

            wType=WeightType.NONE;
        }

        mtext.setText(Integer.toString(mass));

        return w;

    }

    public int load( WeightType wt, int m)
    {
        if( m==0 )return m;
        if( ! carryWeight(wt) )return m;

        if( wType==WeightType.NONE )
        {
            Main.group.getChildren().remove(imgw);

            switch(wt)
            {
                case COAL: {imgw= new ImageView(imgCoal);
                   /* System.out.println("COAL");*/break;}
                case ORE: {imgw= new ImageView(imgOre);
                   /* System.out.println("ORE");*/break;}
                case STEEL: {imgw= new ImageView(imgSteel);
                  /*  System.out.println("STEEL");*/break;}
            }

            Main.group.getChildren().add(imgw);

            double x= imgv.getX();
            double y= imgv.getY();

            imgw.setX(x+wx);
            imgw.setY(y+wy);

            wType=wt;
        }

        if( wType!=wt )return m;

        int deltam= massSize()-mass;
        if( deltam==0 )return m;


        int w= (m<deltam)?m:deltam ;
        mass+=w;
        mtext.setText(Integer.toString(mass));

        return m-w;
    }

    public boolean intersects( double x, double y, double wx, double wy ) {
        if( imgv.intersects(x,y,wx,wy) )return true;

        return false;
    }


}
