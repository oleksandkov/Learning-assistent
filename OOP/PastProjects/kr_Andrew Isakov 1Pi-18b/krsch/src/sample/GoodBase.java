package sample;

import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

public class GoodBase {
  Image i;
  ImageView iv;
  Rectangle r;
  Group g;
Label l;
    GoodBase(double x, double y) throws FileNotFoundException {

        i = new Image(new FileInputStream("src/images/castle_base.png"));
        ImageView imageView = new ImageView(i);
        imageView.setX(50+x);
        imageView.setY(25+y);
        imageView.setFitHeight(250);
        imageView.setFitWidth(270);
        imageView.setPreserveRatio(true); //Setting the preserve ratio of the image view
        this.iv = imageView;


        this.l = new Label("Good");
        this.l.setFont(new Font("Arial", 35));
        this.l.setTextFill(Color.BLACK);
        this.l.setTranslateX(60+x);
        this.l.setTranslateY(290+y);

        this.r = new Rectangle(x+35, y+25, 300, 300);
        this.r.setFill(Color.WHITE);

        this.g = new Group();
        this.g.getChildren().addAll(this.r, this.iv, this.l);
    }
}
