package sample;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.input.InputEvent;
import javafx.stage.Stage;

import java.net.URL;
import java.util.ResourceBundle;

public class generateController implements Initializable {

    @FXML
    private Label scout;
    @FXML
    private Label heavy;
    @FXML
    private Label starship;
    @FXML
    private Label Vtekly;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        this.scout.setText(String.valueOf(Main.scoutCounter));
        this.heavy.setText(String.valueOf(Main.heavyCounter));
        this.starship.setText(String.valueOf(Main.starShipCounter));
    }

    @FXML
    public void closeWindow(InputEvent event){
        final Node source = (Node) event.getSource();
        final Stage stage = (Stage) source.getScene().getWindow();
        stage.close();
    }
}
