package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Violin extends Instrument{
    public Violin(){
        this.instrumentImage= new ImageView(new Image("assets/violin.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(140);
        this.type="Violin";
        this.price = 450;
    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x-40);
        this.instrumentImage.setY(y+20);
    }
    @Override
    public Violin clone() throws CloneNotSupportedException {
        Violin temp = (Violin) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
