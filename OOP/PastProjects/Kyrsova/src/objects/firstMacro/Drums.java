package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Drums extends Instrument{
    public Drums(){
        this.instrumentImage= new ImageView(new Image("assets/bombom.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(180);
        this.type = "Drums";
        this.price = 1000;
    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x-10);
        this.instrumentImage.setY(y+60);
    }
    @Override
    public Drums clone() throws CloneNotSupportedException {
        Drums temp = (Drums) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
