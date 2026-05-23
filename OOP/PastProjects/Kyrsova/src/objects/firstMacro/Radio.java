package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Radio extends Instrument {
    public Radio(){
        this.instrumentImage= new ImageView(new Image("assets/radio.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(50);
        this.type = "radio_!Fun!";
        this.price = 10000000;
    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x+40);
        this.instrumentImage.setY(y+40);
    }
    @Override
    public Radio clone() throws CloneNotSupportedException {
        Radio temp = (Radio) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
