package objects.firstMacro;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Trembita extends Instrument {
    public Trembita(){
        this.instrumentImage= new ImageView(new Image("assets/tremb.png"));
        this.instrumentImage.setPreserveRatio(true);
        this.instrumentImage.setFitHeight(80);
        this.type = "Trembita";
        this.price = 17;

    }
    @Override
    public void update(double x, double y) {
        this.instrumentImage.setX(x+35);
        this.instrumentImage.setY(y-20);
    }
    @Override
    public Trembita clone() throws CloneNotSupportedException {
        Trembita temp = (Trembita) super.clone();
        temp.instrumentImage = new ImageView();
        temp.instrumentImage.setImage(this.instrumentImage.getImage());
        temp.instrumentImage.setPreserveRatio(this.instrumentImage.isPreserveRatio());
        temp.instrumentImage.setFitHeight(this.instrumentImage.getFitHeight());
        return temp;
    }
}
