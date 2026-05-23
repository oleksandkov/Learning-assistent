package sample;


import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;

import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.MenuItem;
import javafx.scene.input.KeyCombination;
import javafx.stage.FileChooser;


import java.io.File;
import java.util.Optional;

public class MenuBarController {

    @FXML
    private MenuItem menuFileOpenSave;
    @FXML
    private MenuItem menuFileSave;
    @FXML
    private MenuItem menuFileExit;

    @FXML
    void initialize() {
        EventHandler<ActionEvent> menuHandler = new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                String buttonName = ((MenuItem) event.getTarget()).getText();
                System.out.println(event.getEventType());
                if (buttonName.equals("Save")) {
                    FileChooser fileChooser = new FileChooser();
                    fileChooser.setTitle("Виберіть місце для збереження");
                    fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("XML-збереження", "*.xml"),
                            new FileChooser.ExtensionFilter("TXT-збереження", "*.txt"),
                            new FileChooser.ExtensionFilter("Усі файли", "*.*"));
                    File file = fileChooser.showSaveDialog(Main.getScene().getWindow());
                    if (file != null)
                        Serialization.serializeNow(file);
                } else if (buttonName.equals("Open save")) {
                    Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
                    alert.setHeaderText("Ви дійсно хочете це зробити?");
                    alert.setContentText("Переконайтеся, що поточна гра збережена");

                    Optional<ButtonType> option = alert.showAndWait();
                    if (option.get() == ButtonType.OK) {
                        FileChooser fileChooser = new FileChooser();
                        fileChooser.setTitle("Виберіть файл");
                        fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("XML-збереження", "*.xml"),
                                new FileChooser.ExtensionFilter("TXT-збереження", "*.txt"),
                                new FileChooser.ExtensionFilter("Усі файли", "*.*"));
                        File file = fileChooser.showOpenDialog(Main.getScene().getWindow());
                        if (file != null)
                            Serialization.deserializeNow(file);
                    }
                } else if (buttonName.equals("Exit")){
                    Platform.exit();
                }
            }
        };

        menuFileOpenSave.setAccelerator(KeyCombination.keyCombination("Ctrl + O"));
        menuFileOpenSave.setOnAction(menuHandler);
        menuFileSave.setAccelerator(KeyCombination.keyCombination("Ctrl + S"));
        menuFileSave.setOnAction(menuHandler);
        menuFileExit.setAccelerator(KeyCombination.keyCombination("Ctrl + E"));
        menuFileExit.setOnAction(menuHandler);
    }
}






