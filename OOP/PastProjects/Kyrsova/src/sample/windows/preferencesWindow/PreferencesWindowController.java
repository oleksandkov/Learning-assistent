package sample.windows.preferencesWindow;

import javafx.fxml.FXML;
import javafx.scene.control.*;

public class PreferencesWindowController {

    private boolean isMap = Preferences.isMAP();
    private boolean isInformation = Preferences.isINFORMATION();
    private double speed = Preferences.getSPEED();
    private double volume = Preferences.getVOLUME();
    private Complexity complexity = Preferences.getCOMPLEXITY();
    @FXML
    private CheckBox checkMiniMap;

    @FXML
    private CheckBox checkInformation;

    @FXML
    private RadioButton radioLight;

    @FXML
    private RadioButton radioMedium;

    @FXML
    private RadioButton radioHard;

    @FXML
    private ScrollBar scrollSpeed;

    @FXML
    private ScrollBar scrollVolume;

    @FXML
    private Button chooseButton;

    @FXML
    private Button cancelButton;

    @FXML
    void initialize() {

        checkMiniMap.setOnAction(event -> {
            isMap = checkMiniMap.isSelected();
        });
        checkMiniMap.setSelected(Preferences.isMAP());

        checkInformation.setOnAction(event -> {
            isInformation = checkInformation.isSelected();
        });
        checkInformation.setSelected(Preferences.isINFORMATION());

        scrollSpeed.setMin(1);
        scrollSpeed.setMax(80);
        scrollSpeed.setValue(speed);
        scrollSpeed.valueProperty().addListener(event -> {
            speed = scrollSpeed.getValue();
        });
        scrollVolume.setMin(0);
        scrollVolume.setMax(1.5);
        scrollVolume.setValue(volume);
        scrollVolume.valueProperty().addListener(event -> {
            volume = scrollVolume.getValue();
        });
        ToggleGroup toggleGroup = new ToggleGroup();
        radioLight.setToggleGroup(toggleGroup);
        radioMedium.setToggleGroup(toggleGroup);
        radioHard.setToggleGroup(toggleGroup);

        radioLight.setOnAction(event -> {
            complexity = Complexity.LIGHT;
        });
        radioMedium.setOnAction(event -> {
            complexity = Complexity.MEDIUM;

        });
        radioHard.setOnAction(event -> {
            complexity = Complexity.HARD;
        });
        switch (complexity) {
            case LIGHT:
                radioLight.fire();
                break;
            case MEDIUM:
                radioMedium.fire();
                break;
            case HARD:
                radioHard.fire();
                break;
        }

        chooseButton.setOnAction(event -> {
            Preferences.setMAP(isMap);
            Preferences.setINFORMATION(isInformation);
            Preferences.setSPEED(speed);
            Preferences.setVOLUME(volume);
            Preferences.setCOMPLEXITY(complexity);
            PreferencesWindow.getWindow().close();
        });
        cancelButton.setOnAction(event -> {
            PreferencesWindow.getWindow().close();
        });

    }
}
