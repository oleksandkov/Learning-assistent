package sample;

import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

public class Drago extends Mario {
    Drago(
            boolean active,
            boolean side,
            double x, double y,
            double s
    ) throws FileNotFoundException {
        super(active,
                side,
                x, y,
                s);
        this.i = new Image(new FileInputStream("src/images/dino.png"));

        speed = new Speed();
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
/*
        Label num = new Label();
        num.setText(Integer.toString(Main.mr.length));
        num.setFont(new Font("Arial", 20));
        this.l = num;
*/
// define side with setSide(); this.l = G or B (Good, Bad)

//        Label hp = new Label();
//        hp.setText("***");
//        hp.setFont(new Font("Arial", 35));
//        this.lhp = hp;
        setHPasteriscs();
        //Creating a Group object
        //Group root = new Group(aura,imageView, num);
        g = new Group(r, ivMar, lhp, l);
// relocating
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

     Drago() throws FileNotFoundException {
         super();
         this.i = new Image(new FileInputStream("src/images/dino.png"));
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
  @Override
     public void Heal(){
        health_step = 2;
        health += health_step;// System.out.println("Heal!");
        setHPasteriscs();
    }
   // @Override
    public void Damage(){
        health_step = 1;
        health -= health_step;//System.out.println("Damage!");
        setHPasteriscs();
    }

        @Override
    public void Damage(int h){
        health_step = h;
        health -= health_step;//System.out.println("Damage!");
        setHPasteriscs();
    }
}
