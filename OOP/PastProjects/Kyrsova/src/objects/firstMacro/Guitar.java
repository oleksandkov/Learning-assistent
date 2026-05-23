package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;


public class Guitar extends Instrument {

    public Guitar(){
        this.instrumentImage= new ImageView(new Image("assets/guitar.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(80);
        this.type = "Guitar";
        this.price = 100;
    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x+10);
        this.instrumentImage.setY(y+70);
    }
    @Override
    public Guitar clone() throws CloneNotSupportedException {
        Guitar temp = (Guitar) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
