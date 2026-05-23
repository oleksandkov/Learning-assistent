package army.window;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import sample.windows.aboutWindow.AboutWindow;

import java.io.IOException;

import static javafx.stage.Modality.APPLICATION_MODAL;

public class EnterNumbersWindow {
    private static Stage window;
    private static Scene scene;

    public static Scene getScene() {
        return scene;
    }
    public static Stage getWindow() {
        return window;
    }
    public EnterNumbersWindow(){
        window = new Stage();
        window.initModality(APPLICATION_MODAL);
        window.setResizable(false);

    }
    public boolean display() throws IOException {
        window.setTitle("Введіть кілкість призовників");
        Parent alert = FXMLLoader.load(EnterNumbersWindow.class.getResource("enterNumbersWindow.fxml"));
        scene = new Scene(alert);
        window.setScene(scene);
        window.getIcons().add(new Image("assets/aboutIcon.png"));
        window.show();
        return true;
    }
}
