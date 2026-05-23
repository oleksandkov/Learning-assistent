package sample.windows.chooseAnInstrumentWindow;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import objects.micro.OrchestraConductor;
import sample.windows.createShopperWindow.CreateShopperWindow;

import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class ChooseAnInstrumentWindow {
    private static Stage window;
    private static OrchestraConductor orchestra;

    public static OrchestraConductor getOrchestraConductor() {
        return orchestra;
    }

    public ChooseAnInstrumentWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);
    }


    public boolean display( OrchestraConductor orchestraConductor) throws IOException {

        orchestra = orchestraConductor;
        window.setTitle("Виберіть інструмент");
        Parent alert = FXMLLoader.load(ChooseAnInstrumentWindow.class.getResource("chooseAnInstrumentWindow.fxml"));
        Scene scene = new Scene(alert);
        window.setScene(scene);
        window.getIcons().add(new Image("assets/aboutIcon.png"));
        window.show();

        return true;
    }

    public static Stage getWindow() {
        return window;
    }
}
