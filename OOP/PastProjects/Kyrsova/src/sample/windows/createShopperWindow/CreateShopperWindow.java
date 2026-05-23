package sample.windows.createShopperWindow;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class CreateShopperWindow {

    private static Stage window;

    public CreateShopperWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);
    }


    public boolean display() throws IOException {

        window.setTitle("Створення нового персонажа");
        Parent alert = FXMLLoader.load(CreateShopperWindow.class.getResource("createShopperWindow.fxml"));
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
