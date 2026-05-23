package sample;

import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.ComboBox;
import javafx.scene.input.InputEvent;
import javafx.stage.Stage;
import sample.micro.Scout;

import java.net.URL;
import java.util.ResourceBundle;

public class GeneralController implements Initializable {


    @FXML
    private ComboBox chooseGeneral;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        for (Scout s : Main.getWorld().getShips()){
            chooseGeneral.getItems().addAll(s);
        }
    }

    public void setGeneral(InputEvent event){
        for (Scout s : Main.getWorld().getShips()){
            if (chooseGeneral.getValue() == s){
                s.setGeneral();
            }
        }
        final Node source = (Node) event.getSource();
        final Stage stage = (Stage) source.getScene().getWindow();
        stage.close();
    }


}