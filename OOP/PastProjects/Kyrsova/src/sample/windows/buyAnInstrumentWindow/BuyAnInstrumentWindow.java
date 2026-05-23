package sample.windows.buyAnInstrumentWindow;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import sample.windows.chooseAnInstrumentWindow.ChooseAnInstrumentWindow;
import sample.windows.createShopperWindow.CreateShopperWindow;

import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class BuyAnInstrumentWindow {
    private static Stage window;
    private static Shopper shopper;

    public static Shopper getShopper() {
        return shopper;
    }

    public BuyAnInstrumentWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);
    }


    public boolean display(Shopper shopper) throws IOException {

        BuyAnInstrumentWindow.shopper = shopper;
        window.setTitle("Магазин інструментів");
        Parent alert = FXMLLoader.load(BuyAnInstrumentWindow.class.getResource("buyAnInstrumentWindow.fxml"));
        Scene scene = new Scene(alert);
        window.setScene(scene);
        window.getIcons().add(new Image("assets/createIcon.png"));
        window.show();

        return true;
    }

    public static Stage getWindow() {
        return window;
    }
}
