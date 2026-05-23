package sample;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.input.InputEvent;
import javafx.stage.Stage;

import java.net.URL;
import java.util.HashSet;
import java.util.ResourceBundle;
import java.util.logging.Level;

public class ConfController implements Initializable {

    @FXML
    private Label name;
    @FXML
    private Label side;
    @FXML
    private Label active;
    @FXML
    private Label level;
    @FXML
    private Label health;
    @FXML
    private Label damage;
    @FXML
    private Label speed;
    @FXML
    private Label x;
    @FXML
    private Label y;




    @Override



    public void initialize(URL url, ResourceBundle resourceBundle) {
        this.name.setText(Main.getScoutConfig().getName().getText());
        this.side.setText(Main.getScoutConfig().getSide());
        this.active.setText(String.valueOf(Main.getScoutConfig().getIsActive()));
        this.level.setText(Main.getScoutConfig().getType());
        this.health.setText(String.valueOf(Main.getScoutConfig().getHealth()));
        this.damage.setText(String.valueOf(Main.getScoutConfig().getDamage()));
        this.speed.setText(String.valueOf(Main.getScoutConfig().getSpeed()));
        this.x.setText(String.valueOf(Main.getScoutConfig().getChordX()));
        this.y.setText(String.valueOf(Main.getScoutConfig().getChordY()));
    }

    @FXML
    public void closeWindow(InputEvent event) {
        final Node source = (Node) event.getSource();
        final Stage stage = (Stage) source.getScene().getWindow();
        stage.close();
    }
}
