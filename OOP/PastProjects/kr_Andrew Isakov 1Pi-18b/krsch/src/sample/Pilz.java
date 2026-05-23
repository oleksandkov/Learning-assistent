package sample;

import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.text.Font;

import java.io.*;


public class Pilz {


    // int side = -1; // 0 or 1 . and -1 if undefined
/*
    public enum side{
        GOOD,
        BAD,
        UNDEFINED
    }

 */
   private int side = 0; // Undefined
    public int getSide() {
        return side;
    }
    public void setSide(int a){
        if (a<=(-1)) {
            this.side = -1;         // -1 Bad
            this.r.setFill(Color.BLACK);
        }
        if (a>= 1) {
            this.side = 1;   // 1 Good
            this.r.setFill(Color.WHITE);
        }
        else if(a == 0) {
            this.side = a;
            this.r.setFill(Color.YELLOW);
        }
    }

    public  Circle r;
    Image i;
    ImageView iv;
    Label l;
    protected Group g;


    double damage;
    static int counter = 0;
    int count=0;

    Pilz(double x, double y) throws FileNotFoundException {
        i = new Image(new FileInputStream("src/images/Pilz.png"));
        ImageView imageView = new ImageView(i);
        imageView.setX(50+x);
        imageView.setY(25+y);
        imageView.setFitHeight(250);
        imageView.setFitWidth(270);
        imageView.setPreserveRatio(true); //Setting the preserve ratio of the image view
        this.iv = imageView;

        Circle aura = new Circle();
        aura.setCenterX(170+x);
        aura.setCenterY(150+y);
        aura.setRadius(150.0f);
        aura.setFill(Color.YELLOW);
        r = aura;
        // my aura

        Label num = new Label();
        num.setText(Integer.toString(counter));   counter++; count = counter;
        num.setFont(new Font("Arial", 20));
        num.setTranslateX(140+x);
        num.setTranslateY(100+y);
        this.l = num;

        //Creating a Group object
        //Group root = new Group(aura,imageView, num);
        this.g = new Group(r, this.iv, this.l );
    }

    Pilz() throws FileNotFoundException {
        double x,y;
        x=y=0;
        i = new Image(new FileInputStream("src/images/Pilz.png"));
        ImageView imageView = new ImageView(i);
        imageView.setX(50+x);
        imageView.setY(25+y);
        imageView.setFitHeight(250);
        imageView.setFitWidth(270);
        imageView.setPreserveRatio(true); //Setting the preserve ratio of the image view
        this.iv = imageView;

        Circle aura = new Circle();
        aura.setCenterX(170+x);
        aura.setCenterY(150+y);
        aura.setRadius(150.0f);
        aura.setFill(Color.YELLOW);
        r = aura;
        // my aura

        Label num = new Label();
        num.setText(Integer.toString(counter));  counter++; count =counter;
        num.setFont(new Font("Arial", 20));
        num.setTranslateX(140+x);
        num.setTranslateY(100+y);
        this.l = num;

        //Creating a Group object
        //Group root = new Group(aura,imageView, num);
        this.g = new Group(r, this.iv, this.l );
    }

    public Group getGroup_Pilz()
    {
        return g;
    }

    // get (Pythagorean theorem) distance from object to Pilz
    // I'll look for the relative min sqrt() doesn't matter
    public double getDistance( double x, double y )            // I moved it to Pilz
    {
        double rezult = (iv.getX() - x)*
                (iv.getX() - x)+
                (iv.getY() - y)*
                        (iv.getY() - y) ;
        return rezult;
    }

    public void Open( BufferedReader bufferedReader ) throws IOException
    {
        String text;
        text = bufferedReader.readLine();
        this.iv.setX(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.iv.setY(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.r.setCenterX(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.r.setCenterY(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.l.setTranslateX(Double.parseDouble(text));
        text = bufferedReader.readLine();
        this.l.setTranslateY(Double.parseDouble(text));
        text = bufferedReader.readLine();
        setSide(Integer.parseInt(text));
        text = bufferedReader.readLine();
        this.count = Integer.parseInt(text);
    }

    // o(boolean active, boolean side, double x, double y, double s) t
   public void Save( FileWriter fileWriter ) throws IOException
    {
        fileWriter.write( Double.toString(iv.getX()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(iv.getY()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(r.getCenterX()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(r.getCenterY()) );
        fileWriter.write("\n");
        //fileWriter.write(           r.getFill().toString());
        //fileWriter.write("\n");
        fileWriter.write( Double.toString(l.getTranslateX()) );
        fileWriter.write("\n");
        fileWriter.write( Double.toString(l.getTranslateY()) );
        fileWriter.write("\n");
        fileWriter.write( Integer.toString(getSide()) );
        fileWriter.write("\n");
        fileWriter.write( Integer.toString(this.count) );
        fileWriter.write("\n");
    }

}
