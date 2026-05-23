package sample;

import javafx.beans.property.ReadOnlyObjectProperty;
import javafx.event.EventHandler;
import javafx.geometry.Bounds;
import javafx.geometry.Point2D;
import javafx.scene.Group;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.text.Font;

import java.io.*;
import java.util.Comparator;
import java.util.Random;

public class Mario implements Cloneable{
    // basic abilities or action triggers?
    protected boolean active = false;
    public boolean isActive() {
        return active;
    }

    protected boolean side = true;
    public boolean getSide() {
        return side;
    }
    public void setSide(boolean side) {
        this.side = side;////;
        if (side == true) this.l.setText("G"); // Good
        else if (side == false) this.l.setText("B"); // Bad
        l.setFont(new Font("Arial", 20));
    }

    protected static boolean to_Base = false;
    protected static boolean rnd_move = false;

    public static void SwitchTo_Base() {Mario.to_Base = !Mario.to_Base;}
    public static void SwitchRnd_Move() {Mario.rnd_move = !Mario.rnd_move;}

    // controled moves
    //protected double speed;
    protected double health = 500;
    protected static double max_health = 500;
    protected double part_h_divider = 5;
    protected double part_h = health/part_h_divider;
    protected double health_step = 1;
    // ?
    static double newx;
    static double newy;
    protected Speed speed = new Speed();
    protected double step = 30;        // in methods Left Right Up Down

    // speed will affect automatic as well as Active (step) movement
    // Helpful things
    static Mario[] tmp;
    static int n = 0;
    static Random random = new Random();
    protected Point2D point = newRANDpntCreator();

    // graphical part
    protected Circle r;
    protected Image i;
    protected ImageView ivMar;
    protected Label l = new Label(), lhp = new Label();
    protected String s;
    protected Group g;
    public Group getGroup() {
        return g;
    }

    // Constructors
    Mario() throws FileNotFoundException {
        this.i = new Image(new FileInputStream("src/images/mario.png"));
//speed.setSpeed(3);
        ImageView imageView = new ImageView(i);
        imageView.setFitHeight(50);
        imageView.setFitWidth(50);
        imageView.setPreserveRatio(true);
        this.ivMar = imageView;

        Circle aura = new Circle();
        aura.setRadius(40.0f);
        aura.setFill(Color.YELLOW);
        this.r = aura;

        setSide(random.nextBoolean());
        setHPasteriscs(); // label
        g = new Group(r, ivMar, lhp, l);
        // relocating
        ivMar.relocate(20, 15);
        lhp.relocate(8, 0);
        l.relocate(0, 45);
        r.relocate(0, 0);

        g.setLayoutX(Math.random() * 1000.0d);
        g.setLayoutY(Math.random() * 1000.0d);

        this.g.setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                switchActivation();
            }
        });
    }

    Mario(boolean active, boolean side, double x, double y, double s) throws FileNotFoundException {
        this.i = new Image(new FileInputStream("src/images/mario.png"));

        speed.setSpeed(s);

        ImageView imageView = new ImageView(i);
        imageView.setFitHeight(50);
        imageView.setFitWidth(50);
        imageView.setPreserveRatio(true);
        this.ivMar = imageView;

        Circle aura = new Circle();
        aura.setRadius(40.0f);
        this.active = active;
        aura.setFill(Color.YELLOW);
        this.r = aura;

        if (active) r.setFill(Color.RED);

        this.setSide(side);

        setHPasteriscs();
        g = new Group(r, ivMar, lhp, l); // relocating
        ivMar.relocate(20, 15);
        lhp.relocate(14, 0);
        l.relocate(0, 45);
        r.relocate(0, 0);

        g.setLayoutX(x);
        g.setLayoutY(y);

        this.g.setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                switchActivation();
            }
        });
    }
    //  'Back-end'
    public void switchActivation() {
        this.active = !this.active;
        if (this.active)
            this.r.setFill(Color.RED);
        else
            this.r.setFill(Color.YELLOW);
    }
    public void setActive(boolean a) {
        this.active = a;
        if (this.active)
            this.r.setFill(Color.RED);
        else
            this.r.setFill(Color.YELLOW);
    }

    public void Left() {
        if (!active) return;
        double y = g.getLayoutX() - step;
        g.setLayoutX(y);
    }

    public void Up() {
        if (!active) return;
        double y = g.getLayoutY() - step;
        g.setLayoutY(y);

    }

    public void Right() {
        if (!active) return;
        double y = g.getLayoutX() + step;
        g.setLayoutX(y);
    }

    public void Down() {
        if (!active) return;
        double y = g.getLayoutY() + step;
        g.setLayoutY(y);
    }


    public static Point2D getNextMoveToPilz(double x, double y) // the (x,y) of the object(Mario)
    {
        double[] distances = new double[Main.pl.length];
        for (int i = 0; i < Main.pl.length; i++) {
            if (Main.pl[i] != null) {
                distances[i] = Main.pl[i].getDistance(x, y);
            } else
                distances[i] = 1000000000.0;
        }
        double mind = 1000000000.0;
        int numd = 0;

        for (int i = 0; i < Main.pl.length; i++) {
            if ((Main.pl[i].getSide() == 0) && distances[i] < mind) {     //Magic here
                // this line allows Marios go from mushroom to mushroom
                mind = distances[i];
                numd = i;
            }
        }
        Point2D p2d = new Point2D(0, 0);

        if (Main.pl[numd] != null)                  // wonder if_ th line below will work
            p2d = new Point2D(Main.pl[numd].iv.getX(), Main.pl[numd].iv.getY());

        return p2d;
    }
    // the Brain
    public void Move() {
        if (isActive()) return;
        if (to_Base == false && Mario.rnd_move == false) {
            Point2D p2d = getNextMoveToPilz(g.getLayoutX(), g.getLayoutY());

            newx = p2d.getX();
            newy = p2d.getY();

            double dx = newx - g.getLayoutX();
            dx = (Math.abs(dx) > speed.getSpeed()) ? Math.signum(dx) * speed.getSpeed() : dx;
            double dy = newy - g.getLayoutY();
            dy = (Math.abs(dy) > speed.getSpeed()) ? Math.signum(dy) * speed.getSpeed() : dy;

            g.setLayoutX(g.getLayoutX() + dx);
            g.setLayoutY(g.getLayoutY() + dy);
        }
        // some random movement
        if (Mario.rnd_move == true && Mario.to_Base == false) {
            int x = (int) g.getLayoutX();
            int y = (int) g.getLayoutY();
            if (x == point.getX() && y == point.getY()) {
                point = newRANDpntCreator();
            }
            int go = 1;
            if (x > point.getX()) {
                g.setLayoutX(g.getLayoutX() - go);
            } else if (y > point.getY()) {
                g.setLayoutY(g.getLayoutY() - go);
            } else if (x < point.getX()) {
                g.setLayoutX(g.getLayoutX() + go);
            } else if (y < point.getY()) {
                g.setLayoutY(g.getLayoutY() + go);
            }
        }
        // to the Base!
        if (Mario.to_Base == true && Mario.rnd_move == false) {
            if (this.getSide() == true) {
                newx = Main.goodBase.iv.getX()+Main.goodBase.r.getWidth()/2;
                newy = Main.goodBase.iv.getY()+Main.goodBase.r.getHeight()/2;

                double dx = newx - g.getLayoutX();
                dx = (Math.abs(dx) > speed.getSpeed()) ? Math.signum(dx) * speed.getSpeed() : dx;
                double dy = newy - g.getLayoutY();
                dy = (Math.abs(dy) > speed.getSpeed()) ? Math.signum(dy) * speed.getSpeed() : dy;

                g.setLayoutX(g.getLayoutX() + dx);
                g.setLayoutY(g.getLayoutY() + dy);
            }
            else {
                newx = Main.badBase.iv.getX()+Main.badBase.r.getWidth()/2;
                newy = Main.badBase.iv.getY()+Main.badBase.r.getHeight()/2;

                double dx = newx - g.getLayoutX();
                dx = (Math.abs(dx) > speed.getSpeed()) ? Math.signum(dx) * speed.getSpeed() : dx;
                double dy = newy - g.getLayoutY();
                dy = (Math.abs(dy) > speed.getSpeed()) ? Math.signum(dy) * speed.getSpeed() : dy;

                g.setLayoutX(g.getLayoutX() + dx);
                g.setLayoutY(g.getLayoutY() + dy);
            }
        }
    }
    // check if Marios intersect
    public static void eatOne(Mario[] mr) {
        ReadOnlyObjectProperty<Bounds> iBounds;
        ReadOnlyObjectProperty<Bounds> jBounds;

        for (int i = 0; i < mr.length; i++) {
            for (int j = 0; j < mr.length; j++) {
                iBounds = mr[i].getGroup().boundsInParentProperty();
                jBounds = mr[j].getGroup().boundsInParentProperty();
                if (iBounds.get().intersects(jBounds.get())) {
                    if (mr[i].getSide() != mr[j].getSide()) {
                        mr[i].Damage(1);        // static polymorphism
                        mr[j].Damage();
                    }
                }
            }
        }
    }

    static int a = 0;
    public static boolean isHealthy(Mario [] mr) {
        Mario.a = 0;
        for (int i = 0; i < mr.length; i++) {
            if ( !(mr[i].health < 1)){
                Mario.a++;
            }
        }
        if(Mario.a < mr.length) return false;
        else return true;                           // if everyone in array is healthy returns TRUE
    }

    public static Mario[] DeleteNull(Mario[] mr, boolean b) {
        // Code below is from KeyHandler in main (DELETE) modified a bit
        Mario.n = 0;
        if(b) return mr;                        // if everyone is healthy
        for (int j = 0; j < mr.length; j++) {
            if (mr[j].health < 1.0) {
                Mario.n++;
                Main.groupMar.getChildren().remove(mr[j].getGroup());
                mr[j].g.setVisible(false);
            }
        }
        Mario.tmp = new Mario[mr.length - Mario.n];
        Mario.n = 0;
        for (int k = 0; k < mr.length; k++) {
            if ((mr[k].health < 1.0) == false) {
                tmp[n] = mr[k];
                ++Mario.n;
            }
        }
        return Mario.tmp;
    }

    public Point2D newRANDpntCreator() {
        Random rnd = new Random();
        int x = rnd.nextInt( (int)Main.wp.i.getWidth());
        int y = rnd.nextInt( (int)Main.wp.i.getHeight());

        Point2D point = new Point2D(x, y);
        return point;
    }

    public void Heal(){
        health_step = 1;
        health += health_step;// System.out.println("Heal!");
        setHPasteriscs();
    }
    // is easily damaged
    public void Damage(){
        health_step = 2;
        health -= health_step;//System.out.println("Damage!");
        setHPasteriscs();
    }

    public void Damage(int health_step){
        health -= health_step;//System.out.println("Damage!");
        setHPasteriscs();
    }
    public void setHPasteriscs() {
        lhp.setFont(new Font("Arial", 45));
        s = "*****";
        if (health == max_health) lhp.setText(s);
        if (health <= 4 * part_h) lhp.setText("****");
        if (health <= 3 * part_h) lhp.setText("***");
        if (health <= 2 * part_h) lhp.setText("**");
        if (health <= part_h) lhp.setText("*");
    }

    public static void touchcheck(Mario[] mr, Pilz[] pl) {
        for (int i = 0; i < mr.length; i++) {
            // if(mr[i] == null) continue;
            for (int j = 0; j < pl.length; j++) {
                //       if(pl[j] == null) continue;
                if (mr[i].getGroup().getBoundsInParent().intersects
                        (pl[j].getGroup_Pilz().getBoundsInParent())
                        || pl[j].getGroup_Pilz().getBoundsInParent().contains
                        (mr[i].getGroup().getBoundsInParent())
                ) {
                    // get side from Mario, make Pilz side the same as Mario
                    // See ELSE statement below
                    // better to arrange into different method
                    if(pl[j].getSide() != 0) {
                        if((mr[i].getSide() == true && pl[j].getSide() == 1)) mr[i].Heal();     // dynamic polimorphism here
                        else if (mr[i].getSide() == false && pl[j].getSide() == -1) mr[i].Heal();
                        else mr[i].Damage();
                    }
                    else{
                        if (mr[i].getSide() == true) pl[j].setSide(1);
                        else                 pl[j].setSide(-1);
                    }
                }
            }
            // in the base
            if (mr[i].getGroup().getBoundsInParent().intersects
                    (Main.goodBase.g.getBoundsInParent())) {
                if(mr[i].getSide() == true) {
                    mr[i].health = max_health;
                    mr[i].setHPasteriscs();}
                else mr[i].Damage();
            }
            if(mr[i].getGroup().getBoundsInParent().intersects
                    (Main.badBase.g.getBoundsInParent())){
                if(mr[i].getSide() == false) {
                    mr[i].health = max_health;
                    mr[i].setHPasteriscs();}
                else mr[i].Damage();

            }
            // touch to the walls
            if (mr[i].getGroup().getBoundsInParent().getMinX() <=0 ||
                    mr[i].getGroup().getBoundsInParent().getMinY() <=0 ||
                    mr[i].getGroup().getBoundsInParent().getMinY() >= Main.wp.border.getHeight()-70 ||
                    mr[i].getGroup().getBoundsInParent().getMinX() >= Main.wp.border.getWidth()-70
            ){mr[i].active = false; mr[i].r.setFill(Color.YELLOW);}
        }
    }


    public int compareTo(Mario mr2) {
        if(this.speed.equals(mr2.speed)) {
            if( this.getGroup().getLayoutX()*this.getGroup().getLayoutY() >
                    mr2.getGroup().getLayoutX()*mr2.getGroup().getLayoutY()) return 1;

            if( this.getGroup().getLayoutX()*this.getGroup().getLayoutY() <
                    mr2.getGroup().getLayoutX()*mr2.getGroup().getLayoutY()) return -1;

            return 0;
        }
        else return this.speed.compareTo(mr2.speed);
    }

    public static void compar(){
        for(int i=0; i< Main.mr.length;i++) {
            for (int j = 0; j < Main.mr.length; j++) {
                Main.mr[i].compareTo(Main.mr[j]);
                java.util.Arrays.sort(Main.mr, Mario.SpeedComparator);

                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setTitle("Comparing all objects.");
                alert.setHeaderText("Results:");
                alert.setContentText("Clonning was successful!");

                alert.showAndWait();
            }
        }
    }
    // Comparing speed
    public static Comparator<Mario> SpeedComparator
            = new Comparator<Mario>() {
        @Override
        public int compare(Mario o1, Mario o2) {
            return o1.speed.compareTo(o2.speed);
        }
    };

    @Override
    public Mario clone() throws CloneNotSupportedException
    {
        this.switchActivation();
        // Main.groupMar.getChildren().add(this.getGroup()) ;
        Mario cloned = (Mario)super.clone();
        cloned.speed =  (Speed) this.speed.clone();
        return cloned;
    }

    @Override
    public String toString()
    {
        String side;
        if(getSide() == true) side = "Good";
        else side = "Bad";
        String clas = null;
//        System.out.println(this.getClass().toString());
        if((this.getClass().toString().equals("class sample.Mario"))){clas = "Mario";}
        if((this.getClass().toString().equals("class sample.Drago"))){clas = "Drago";}
        if((this.getClass().toString().equals("class sample.Princess"))){clas = "Princess";}
        String rez= "\n[ "+clas+", "+side+", " +
                Integer.toString((int)getGroup().getLayoutX())+", " +
                Integer.toString((int)getGroup().getLayoutY())+", " +
                Integer.toString((int)speed.getSpeed())+" ]";
        return rez;
    }

    //      de/serialisation
// o(boolean active, boolean side, double x, double y, double s) t
    public void Open( BufferedReader bufferedReader ) throws IOException
    {
        String text;

        text = bufferedReader.readLine();
        boolean act= (text.equals("true"))?true:false;
        setActive(act);
        text = bufferedReader.readLine();
        act= (text.equals("true"))?true:false;
        setSide(act);
        text = bufferedReader.readLine();
        this.getGroup().setLayoutX(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.getGroup().setLayoutY(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.speed.setSpeed(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.health = (Double.parseDouble(text));
        setHPasteriscs();
    }

    // o(boolean active, boolean side, double x, double y, double s) t
    public void Save( FileWriter fileWriter ) throws IOException
    {
        fileWriter.write(getKlas()); // first level = Mario
        fileWriter.write("\n");
        fileWriter.write( Boolean.toString(isActive()));
//          fileWriter.write( (isActive())?"true":"false" );
//        fileWriter.write("\n");
        fileWriter.write("\n");
        fileWriter.write( Boolean.toString(getSide()));
        fileWriter.write("\n");
        fileWriter.write( Double.toString(getGroup().getLayoutX()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(getGroup().getLayoutY()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(speed.getSpeed()));
        fileWriter.write("\n");
        fileWriter.write( Double.toString(this.health) );
        fileWriter.write("\n");
    }


    public String getKlas(){
        String clas = null;
        if((this.getClass().toString().equals("class sample.Mario"))){clas = "1";}
        if((this.getClass().toString().equals("class sample.Drago"))){clas = "2";}
        if((this.getClass().toString().equals("class sample.Princess"))){clas = "3";}
        return clas;
    }
}

//public void Save( FileWriter fileWriter ) throws IOException
//    {
//        fileWriter.write(getKlas()); // first level = Mario
//        fileWriter.write( Double.toString(g.getLayoutX()) );
//        fileWriter.write("\n");
//        fileWriter.write( Double.toString(g.getLayoutY()) );
//        fileWriter.write("\n");
//        fileWriter.write( Integer.toString(getSide()) );
//        fileWriter.write("\n");
//    }
