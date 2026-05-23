package sample.windows.aboutWindow;

import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.input.KeyEvent;
import javafx.stage.Stage;

import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class AboutWindow {

    private static Stage window;
    private static Scene scene;

    public static Scene getScene() {
        return scene;
    }
    public static Stage getWindow() {
        return window;
    }
    public AboutWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);

    }


    public boolean display() throws IOException {

        window.setTitle("Про програму");
        Parent alert = FXMLLoader.load(AboutWindow.class.getResource("aboutWindow.fxml"));
        scene = new Scene(alert);
        window.setScene(scene);
        window.getIcons().add(new Image("assets/aboutIcon.png"));
        window.show();

        return true;
    }


}
