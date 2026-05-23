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
import sample.windows.chooseAnEducationWindow.ChooseAnEducationWindow;
import sample.windows.chooseAnInstrumentWindow.ChooseAnInstrumentWindow;

import java.awt.*;
import java.io.IOException;

public class School extends Building {
    public School() {
        this.xChord = Main.random.nextInt(4000);
        this.yChord = Main.random.nextInt(2000);
        this.buildingType = "School";
        Building.numberOfBuildings++;
        this.buildingImage = new ImageView(new Image("assets/school.png"));
        this.buildingImage.setPreserveRatio(true);
        this.buildingImage.setFitHeight(360);

        this.buildingText = new Label("Музична школа, де вас навчать!!!");
        this.buildingText.setFont(new Font("Segoe UI Black Italic", 17));
        this.buildingText.setBorder(new Border(new BorderStroke(Color.valueOf("#C64706"), BorderStrokeStyle.SOLID, new CornerRadii(0), new BorderWidths(3))));

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
            if (!(shopper instanceof OrchestraConductor) && shopper.getInstrument() != null) {
                try {
                    new ChooseAnEducationWindow().display(shopper);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            else {
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setContentText("Ви уже максимально навчені, якщо ж ні - перевірте наявність інструменту");
                alert.showAndWait();
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
        this.buildingText.setLayoutX(x+33 );
        this.buildingText.setLayoutY(y + 323);
        this.shadow.setLayoutX(x + 90);
        this.shadow.setLayoutY(y + 270);
    }
}
