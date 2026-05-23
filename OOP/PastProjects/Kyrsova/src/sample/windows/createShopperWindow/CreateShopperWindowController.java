package sample.windows.createShopperWindow;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import objects.firstMacro.Instrument;
import objects.micro.MusicianMaster;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import sample.Main;



public class CreateShopperWindowController {


    private boolean isMan ;
    private String tempChange = "shopper";
    private String typeOfInstrument = "Nothing";
    @FXML
    private TextField fieldName;

    @FXML
    private TextField fieldMoney;

    @FXML
    private Button yesButton;

    @FXML
    private RadioButton radioMan;

    @FXML
    private RadioButton radioWoman;

    @FXML
    private ListView<String> instrumentsList;

    @FXML
    private RadioButton radioShopper;

    @FXML
    private RadioButton radioMaster;

    @FXML
    private RadioButton orchestra;

    @FXML
    private Label labelUnderInstruments;

    @FXML
    private CheckBox checkIsActive;

    @FXML
    void initialize() {


        ToggleGroup toggleGroup = new ToggleGroup();
        radioMan.setToggleGroup(toggleGroup);
        radioWoman.setToggleGroup(toggleGroup);
        radioMan.setOnAction((event -> {
            isMan = true;
        }));

        radioWoman.setOnAction((event -> {
            isMan = false;
        }));

        ToggleGroup toggleGroup1 = new ToggleGroup();
        radioShopper.setToggleGroup(toggleGroup1);
        radioMaster.setToggleGroup(toggleGroup1);
        orchestra.setToggleGroup(toggleGroup1);
        radioShopper.setOnAction((event -> {
            radioWoman.setLayoutX(93);
            tempChange = "shopper";
            instrumentsList.setOpacity(0);
            labelUnderInstruments.setLayoutX(128);
        }));
        radioMaster.setOnAction((event -> {
            radioWoman.setLayoutX(93);
            tempChange = "master";
            instrumentsList.setOpacity(1);
            labelUnderInstruments.setLayoutX(2000);


        }));
        orchestra.setOnAction((event -> {
            radioWoman.setLayoutX(2000);
            instrumentsList.setOpacity(1);
            labelUnderInstruments.setLayoutX(2000);
            tempChange = "orchestra";
        }));


        ObservableList<String> instruments = FXCollections.observableArrayList("Nothing","Guitar","Drums","Bayan","Piano","Violin", "Trembita" );
        instrumentsList.getItems().addAll(instruments);
        instrumentsList.getSelectionModel().selectedItemProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observable, String oldValue, String newValue) {
                typeOfInstrument = newValue;
            }
        });

        radioMan.fire();
        radioShopper.fire();

        yesButton.setOnAction(event -> {
            if (!fieldName.getText().isEmpty() && !fieldMoney.getText().isEmpty()){
                Shopper temp = null;
                if (tempChange.equals("shopper")){
                    temp = new Shopper(null,checkIsActive.isSelected(), fieldName.getText(), Integer.parseInt(fieldMoney.getText()),isMan);
                } else if (tempChange.equals("master")){
                    temp = new MusicianMaster(Instrument.getInstrument(typeOfInstrument),checkIsActive.isSelected(), fieldName.getText(), Integer.parseInt(fieldMoney.getText()),isMan);
                } else if (tempChange.equals("orchestra")){
                    temp = new OrchestraConductor(Instrument.getInstrument(typeOfInstrument),checkIsActive.isSelected(), fieldName.getText(), Integer.parseInt(fieldMoney.getText()));

                }
                Main.getCity().addNewShopper(temp, true);
                CreateShopperWindow.getWindow().close();
            }
        });
    }
}
