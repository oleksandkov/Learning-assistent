package objects.secondMacro;

import javafx.scene.Group;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.effect.GaussianBlur;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Ellipse;
import javafx.scene.text.Font;
import javafx.scene.transform.Rotate;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import sample.Main;
import sample.windows.buyAnInstrumentWindow.BuyAnInstrumentWindow;

import java.io.IOException;

public class Shop extends Building {
    public Shop() {
        this.xChord = Main.random.nextInt(4000);
        this.yChord = Main.random.nextInt(2000);
        this.buildingType = "Shop";
        Building.numberOfBuildings++;
        this.buildingImage = new ImageView(new Image("assets/shop.png"));
        this.buildingImage.setPreserveRatio(true);
        this.buildingImage.setFitHeight(270);
        this.buildingText = new Label("Магазин музичних інструментів");
        this.buildingText.setFont(new Font("Segoe UI Black Italic", 18));
        this.buildingText.setBorder(new Border(new BorderStroke(Color.valueOf("#462311"), BorderStrokeStyle.SOLID, new CornerRadii(20), new BorderWidths(5))));
        this.shadow = new Ellipse(175, 40);
        this.shadow.setFill(Color.BLACK);
        this.shadow.setOpacity(0.8);
        this.shadow.getTransforms().add(new Rotate(10));
        this.shadow.setEffect(new GaussianBlur(40));


        this.buildingPicture = new Group(shadow, buildingImage, buildingText);
    }
    @Override
    public boolean interact(Shopper shopper, boolean isShiftDown) {
        if (this.getBuildingImage().getLayoutBounds().contains(shopper.getShopperImage().getLayoutBounds())) {
            if (!isShiftDown) {
                if (shopper instanceof OrchestraConductor || shopper.getInstrument() == null) {
                    try {
                        new BuyAnInstrumentWindow().display( shopper);
                    } catch (IOException e) {
                        e.printStackTrace();

                    }
                } else {
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);

                    alert.setTitle("Увага!");
                    alert.setHeaderText("Повертайтеся пізніше");
                    alert.setContentText("Неможливо купити: даний музикант не може володіти більше, ніж одним інструментом!");
                    alert.showAndWait();
                }
            } else {
                shopper.sellAnInstrument();
            }
            return true;
        }
        return false;
    }

    @Override
    public void setBuildingInChords() {
        double x = this.xChord;
        double y = this.yChord;
        this.buildingImage.setX(x);
        this.buildingImage.setY(y);
        this.buildingText.setLayoutX(x +10);
        this.buildingText.setLayoutY(y + 280);
        this.shadow.setLayoutX(x + 90);
        this.shadow.setLayoutY(y + 230);
    }
}
