package sample;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.input.InputEvent;
import javafx.stage.Stage;
import sample.micro.Heavy;
import sample.micro.Scout;
import sample.micro.StarShip;

import java.net.URL;
import java.util.ResourceBundle;

import static sample.Main.random;

public class characterController implements Initializable {
    @FXML
    public TextField textName;
    public TextField count;
    public TextField posY;
    public TextField posX;
    public CheckBox active;
    public RadioButton jediSide;
    public RadioButton sithSide;
    public Button closeButton;
    public ComboBox shipColor;

    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        shipColor.getItems().addAll("Scout", "Heavy", "StarShip");
    }
    @FXML
    public void closeWindow(InputEvent event) {
        final Node source = (Node) event.getSource();
        final Stage stage = (Stage) source.getScene().getWindow();
        stage.close();
    }
@FXML
    public void print(InputEvent event){

//    if (shipColor.getValue().equals("Scout"))
//        this.type = "Scout";
//    else if (shipColor.getValue().equals("Heavy"))
//        this.type = "Heavy";
//    else if (shipColor.getValue().equals("StarShip"))
//        this.type = "StarShip";

    String name = textName.getText();
     String Y = posY.getText();
     String X = posX.getText();
     String countS = count.getText();
     int positionX, positionY, count;
     String side = null;
     boolean status;
    //+++++++++++++++++++++++++++
     try{
         positionX = Integer.parseInt(X);
     }catch (NumberFormatException e){
         positionX = random.nextInt(4000);
     }
    //+++++++++++++++++++++++++++
    try{
        positionY = Integer.parseInt(Y);
    }catch (NumberFormatException e){
        positionY = random.nextInt(2000);
    }
    //+++++++++++++++++++++++++++
    try{
        count = Integer.parseInt(countS);
    }catch (NumberFormatException e){
        count = 1;
    }
    //+++++++++++++++++++++++++++
    if (jediSide.isSelected() && sithSide.isSelected()){
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Error!!!");

        alert.setHeaderText(null);
        alert.setContentText("Choose only one side!!!");

        alert.showAndWait();
        }
    if (jediSide.isSelected()){
        side = "Green";
    }
    if (sithSide.isSelected()){
        side = "Red";
    }

    if (active.isSelected()){
        status = true;
    }else{
        status = false;
    }
    if(textName.getText().isEmpty()){
        name = Main.getNamesArray()[random.nextInt(Main.getNamesArray().length)];
    }
    for (int i = 0; i<count; ++i){
        switch (shipColor.getValue().toString()){
            case "Scout":
                Main.getWorld().addNewShip(new Scout(name,status,5,500,5,200,side,positionX,positionY),false);
                break;
            case "Heavy":
                Main.getWorld().addNewShip(new Heavy(name,status,8,1000,8,200,side,positionX,positionY),false);
                break;
            case "StarShip":
                Main.getWorld().addNewShip(new StarShip(name,status,10,2000,11,200,side,positionX,positionY),false);
                break;
        }
    }

    final Node source = (Node) event.getSource();
    final Stage stage = (Stage) source.getScene().getWindow();
    stage.close();

    }
}
