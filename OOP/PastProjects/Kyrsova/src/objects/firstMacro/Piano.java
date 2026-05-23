package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Piano extends Instrument{
    public Piano(){
        this.instrumentImage= new ImageView(new Image("assets/piano.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(170);
        this.type = "Piano";
        this.price = 600;
    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x+50);
        this.instrumentImage.setY(y+60);
    }
    @Override
    public Piano clone() throws CloneNotSupportedException {
        Piano temp = (Piano) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
