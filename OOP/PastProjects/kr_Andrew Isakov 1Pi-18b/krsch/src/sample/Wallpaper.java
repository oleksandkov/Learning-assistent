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

public class Wallpaper {
    Image i;
    public  static ImageView iv;
    Label l;
    private static Group subi;
    public static Rectangle border;
    static Group Tafel = new Group();
    static Group g;
    Wallpaper() throws FileNotFoundException {
        i = new Image(new FileInputStream("src/images/Mwallpaper.png"));
        ImageView imageView = new ImageView(i);
        imageView.setFitWidth(4600);
        imageView.setFitHeight(4000);
        this.iv = imageView;

//        l = new Label();
//        l.setFont(new Font("Arial", 40));
//        l.setText("Behold the Mario World Map!");
//        //l.setTranslateX(200);
//        //l.setTranslateY(45);

        border = new Rectangle(0,0,this.iv.getFitWidth(), this.iv.getFitHeight());
        border.setFill(Color.TRANSPARENT);
        border.setStroke(Color.RED);

        //Creating a Group object
        //Group root = new Group(aura,imageView, num);
        subi = new Group(this.iv,border);
        Group sub = new Group( subi, Tafel);
        // better to put all the on screen timers into a group located in Main
        this.g = sub;
    }

    public static Group getWallGrp() {
        return g;
    }
}
