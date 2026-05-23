package sample.windows.preferencesWindow;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class PreferencesWindow {
    private static Stage window;

    public PreferencesWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);

    }


    public boolean display() throws IOException {

        window.setTitle("Налаштування");
        Parent alert = FXMLLoader.load(PreferencesWindow.class.getResource("preferencesWindow.fxml"));
        Scene scene = new Scene(alert);
        window.setScene(scene);
        window.getIcons().add(new Image("assets/prefIcon.png"));
        window.show();

        return true;
    }

    public static Stage getWindow() {
        return window;
    }
}
