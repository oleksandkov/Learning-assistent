package sample;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class HeavyCar extends MediumCar {
    public static Image imgHeavyCar;

    public HeavyCar(double x, double y) {
        super();

        imgv = new ImageView(imgHeavyCar);

        Main.group.getChildren().add(imgv);
        imgv.setX(x);
        imgv.setY(y);

        miniImgv = new ImageView(imgHeavyCar);
        miniImgv.setPreserveRatio(true);
        miniImgv.setFitHeight(10.0);
        Main.miniGroup.getChildren().add(miniImgv);

        miniImgv.setX(x*Main.SCALE);
        miniImgv.setY(y*Main.SCALE);

        init();
    }

    @Override
    public boolean carryWeight(WeightType wt) {
        return true;
    }

    @Override
    public int massSize() {
        return 200;
    }

    @Override
    public double speedX() {
        return 8.0;
    }

    @Override
    public double speedY() {
        return 8.0;
    }

    @Override
    public String stringCarType() {
        return "HeavyCar";
    }
}
