package org.example.laba5;

import java.io.File;
import java.util.Optional;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

public class SerializationDialog {

    public static void showDialog(Stage owner) {
        ButtonType btnSave   = new ButtonType("Save",   ButtonBar.ButtonData.LEFT);
        ButtonType btnImport = new ButtonType("Import", ButtonBar.ButtonData.LEFT);
        ButtonType btnCancel = new ButtonType("Cancel", ButtonBar.ButtonData.CANCEL_CLOSE);

        Alert alert = new Alert(Alert.AlertType.NONE, "What would you like to do?", btnSave, btnImport, btnCancel);
        alert.setTitle("Save / Import");
        alert.setHeaderText("Game Serialization (XML)");

        Optional<ButtonType> result = alert.showAndWait();
        if (result.isEmpty() || result.get() == btnCancel) return;

        boolean isSave = result.get() == btnSave;

        FileChooser fc = new FileChooser();
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("XML Files (*.xml)", "*.xml"));
        fc.setTitle(isSave ? "Save Game as XML" : "Import Game from XML");

        File file = isSave ? fc.showSaveDialog(owner) : fc.showOpenDialog(owner);
        if (file == null) return;

        if (isSave && !file.getAbsolutePath().toLowerCase().endsWith(".xml")) {
            file = new File(file.getAbsolutePath() + ".xml");
        }

        if (isSave) {
            GameSerializer.save(file, "XML");
        } else {
            GameSerializer.load(file, "XML");
        }
    }
}
