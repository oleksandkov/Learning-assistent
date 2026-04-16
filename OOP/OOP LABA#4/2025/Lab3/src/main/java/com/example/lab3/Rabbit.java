package com.example.lab3;

import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

public class Rabbit implements Cloneable  {
    private Label name;
    private Stomach stomach;
    private double health;
    private static double healthMax=100;
    private Line life;
    private ImageView irabbit;
    private double x, y;

    private double aimx;
    private double aimy;

    private boolean isActive;
    private boolean isProcessing;

    private Rectangle rectActive;

    public static double imageDeltaX(){
        return 0.0;
    }
    public static double imageDeltaY(){
        return 15+7;
    }
    public Rabbit(String n, double h, double _x, double _y ){
        x=_x;
        y=_y;

        clearAim();

        name=new Label(n);
        name.setLayoutX(x);
        name.setLayoutY(y);
        name.setTextFill(Color.GREEN);

        stomach=new Stomach(0,x,y);

        health=h;
        life=new Line(x,y+15, x+health,y+15);
        life.setStrokeWidth(5);
        life.setStroke(Color.LIGHTGREEN);

        irabbit=new ImageView(World.imgrabbit);
        irabbit.setX(x+imageDeltaX());
        irabbit.setY(y+imageDeltaY());

        isActive = false;
        isProcessing = false;

        rectActive= new Rectangle(x-5,y-5,105,105);
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStroke(Color.MAGENTA);

    }

    public double getImageX(){ return irabbit.getX(); }
    public double getImageY(){ return irabbit.getY(); }
    public double getImageWX(){
        return World.imgrabbit.getWidth(); }

    public double getImageWY(){ return World.imgrabbit.getHeight();}

    @Override
    protected Object clone() throws CloneNotSupportedException {
        Rabbit tmp= new Rabbit(this.getName(), this.getHealth(), this.getX(), this.getY());

        tmp.stomach= (Stomach)this.stomach.clone();

        return tmp;
        //return super.clone();
    }

    public void resurrect(){
        World.group.getChildren().addAll(name, life, irabbit, stomach.getLabel());

        if(isActive)World.group.getChildren().add(rectActive);
    }

    public void harakiri(){
        World.group.getChildren().removeAll(name, life, irabbit, stomach.getLabel());

        if(isActive)World.group.getChildren().remove(rectActive);

    }

    public void eat(int food ){
        stomach.eat(food);
    }

    public Bounds getBoundsInParent(){
        return irabbit.getBoundsInParent();
    }
    public boolean isActive(){
        return isActive;
    }
    public boolean isProcessed(){
        return isProcessing;
    }
    public boolean flipActivation(){
        if(isActive){
            World.group.getChildren().remove(rectActive);
        }
        else{
            World.group.getChildren().add(rectActive);
        }
        isActive = !isActive;

        return isActive;
    }

    public void cancelActivation(){
        if( !isActive() )return;

        flipActivation();
    }
    public boolean mousePrimaryActivate( double mx, double my ){
        if(irabbit.boundsInParentProperty().get().contains(mx,my)){
            flipActivation();
            return true;
        }
        return false;
    }

    @Override
    public String toString() {
        return "Rabbit{" +
                "name=" + name.getText() +
                ", stomach="+ stomach.getFood() +
                ", health=" + health +
                ", x=" + x +
                ", y=" + y +
                '}';
    }
    public String getName(){
        return name.getText();
    }
    public void setName(String n){
        name.setText(n);
    }
    public String getHealthStr(){
        return Double.toString(health) ;
    }
    public double getHealth(){
        return health ;
    }
    public void setHealth(double h){
        health =h;

        life.setStartX(x);
        life.setStartY(y+15);
        life.setEndX(x+health);
        life.setEndY(y+15);

    }
    public void setHealth(String h){

        try {
            health= Double.parseDouble(h);
        }
        catch(Exception e){
            health=0.0;
        }

        life.setStartX(x);
        life.setStartY(y+15);
        life.setEndX(x+health);
        life.setEndY(y+15);

//        World.group.getChildren().remove(life);
//        life=new Line(x,y+15, x+health,y+15);
//        life.setStrokeWidth(5);
//        life.setStroke(Color.LIGHTGREEN);
//        World.group.getChildren().add(life);
    }

    public void setCoordinates(){
        name.setLayoutX(x);
        name.setLayoutY(y);

        stomach.setCoordinates(x, y);

        life.setStartX(x);
        life.setStartY(y+15);
        life.setEndX(x+health);
        life.setEndY(y+15);

        //        World.group.getChildren().remove(life);
//        life=new Line(x,y+15, x+health,y+15);
//        life.setStrokeWidth(5);
//        life.setStroke(Color.LIGHTGREEN);
//        World.group.getChildren().add(life);

        irabbit.setX(x+imageDeltaX());
        irabbit.setY(y+imageDeltaY());

        rectActive.setX(x-5);
        rectActive.setY(y-5);

    }


    public void setX( double _x ){
        x= _x;

        setCoordinates();
    }
    public void setX(String sX){
        try {
            x= Double.parseDouble(sX);
        }
        catch(Exception e){
            x=0.0;
        }

        setX(x);
    }
    public String getXStr(){
        return Double.toString(x);
    }
    public String getYStr(){
        return Double.toString(y);
    }

    public double getX(){
        return x;
    }
    public double getY(){ return y; }

    public void setY( double _y ){
        y= _y;

        setCoordinates();
    }

    public void setY(String sY) {
        try {
            y = Double.parseDouble(sY);
        } catch (Exception e) {
            y = 0.0;
        }

        setY(y);
    }

    public double speedX(){
        return 2.0;
    }
    public double speedY(){
        return 2.0;
    }
    public void clearAim()
    {
        aimx = aimy = -1000.0;
    }

    public void setProcessing()
    {
        isProcessing=true;
    }
    public void clearProcessing()
    {
        isProcessing=false;
    }

    public void setAim( double ax, double ay ){
        aimx = ax;
        aimy = ay;
        //System.out.println("setAim( double ax, double ay )"+ax+" "+ay);
    }
    public boolean isEmptyAim()
    {
        if( (aimx<0) && (aimy<0) )return true;

        return false;
    }

    public void feed( int food ){
        stomach.eat(food);
    }
    public boolean isFullStomach(){
        return stomach.isFullStomach();
    }
    public boolean isHungry(){
       return stomach.isHungry();
    }
    public boolean isDead(){
        if(health>0)return false;

        return true;
    }
    public boolean physiology(){
        if( !stomach.useEnergy() ){

            if(health>0)--health;

            setHealth(health);
            return false;    //Not safe - starving!!!
        }
        return true; // Safe!
    }

    public boolean isFullHealth(){
       if( health < healthMax )return false;

       return true;
    }
    public boolean tryToHeal(){
        if( stomach.isHungry() ) return false;

        if( health<healthMax ){
            ++health;
            setHealth(health);
            return true;
        }

        return false; // healthy!!!
    }
    public void autoMove(){
        //double x = imgv.getX();
        //double y = imgv.getY();

        if( isActive )return;

        if( isProcessing )return;

        if( isEmptyAim() ) {

            World.askWorldwhatToDo(this);
        }
        else {


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

                move(dx, dy);
            }
        }
    }
    public void moveKey( double dx, double dy ){

        if( isProcessing )
        {
            World.askWorldfreeStables(this);
            clearProcessing();
        }

        clearAim();

        x=x+dx;
        y=y+dy;
        setCoordinates();
    }

    public void move( double dx, double dy ){

        x=x+dx;
        y=y+dy;
        setCoordinates();
    }

}
