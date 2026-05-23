package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class MediumCar extends LightCar {
    public static Image imgMediumCar;


    public MediumCar( double x, double y) {
            super();
            imgv = new ImageView(imgMediumCar);

            Main.group.getChildren().add(imgv);
            imgv.setX(x);
            imgv.setY(y);

            init();
    }

    public MediumCar()
    {

    }

    @Override
    public boolean carryWeight(WeightType wt) {
        if( (wt == WeightType.COAL) || (wt == WeightType.ORE) )return true;

        return false;
    }

    @Override
    public String stringCarType() {
        return "MediumCar";
    }

    @Override
    public int massSize() {
        return 100;
    }

    @Override
    public double speedX() {
        return 6.0;
    }

    @Override
    public double speedY() {
        return 6.0;
    }
}

