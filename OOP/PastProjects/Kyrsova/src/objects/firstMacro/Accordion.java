package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Accordion extends Instrument {
    public Accordion(){
        this.instrumentImage= new ImageView(new Image("assets/bayan.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(80);
        this.type = "Bayan";
        this.price = 400;
    }

    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x+10);
        this.instrumentImage.setY(y+60);
    }

    @Override
    public Accordion clone() throws CloneNotSupportedException {
        Accordion temp = (Accordion)super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
