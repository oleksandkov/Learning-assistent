package org.example.laba5;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Separator;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;
import org.example.laba5.Unit.Unit;
import org.example.laba5.Unit.Warrior;

public class UnitCreationDialog {
    private Stage stage;
    private Unit result = null;
    private boolean confirmed = false;

    private ComboBox<String> unitTypeCombo;
    private TextField healthField;
    private TextField damageField;
    private CheckBox spawnedCheckBox;
    private RadioButton allyRadio;
    private TextField inventorField;
    private HBox oreBoxRow;
    private TextField oreField;
    private HBox killsBox;
    private TextField killsField;

    public UnitCreationDialog() {
        createDialog();
    }

    private void createDialog() {
        stage = new Stage();
        stage.initModality(Modality.APPLICATION_MODAL);
        stage.setTitle("Create Unit");
        stage.setWidth(300);
        stage.setHeight(320);
        stage.setResizable(false);

        VBox root = new VBox(8);
        root.setPadding(new Insets(12));

        HBox typeBox = new HBox(8);
        typeBox.getChildren().add(new Label("Type:"));
        unitTypeCombo = new ComboBox<>();
        unitTypeCombo.getItems().addAll("Warrior", "Centurio", "Pretorio");
        unitTypeCombo.setValue("Warrior");
        unitTypeCombo.setPrefWidth(100);
        unitTypeCombo.setOnAction(e -> updateOreVisibility());
        typeBox.getChildren().add(unitTypeCombo);

        HBox healthBox = new HBox(8);
        healthBox.getChildren().add(new Label("Health:"));
        healthField = new TextField();
        healthField.setText("100");
        healthField.setPrefWidth(100);
        healthField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        healthBox.getChildren().add(healthField);

        HBox damageBox = new HBox(8);
        damageBox.getChildren().add(new Label("Damage:"));
        damageField = new TextField();
        damageField.setText("5");
        damageField.setPrefWidth(100);
        damageField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        damageBox.getChildren().add(damageField);

        HBox teamBox = new HBox(15);
        teamBox.getChildren().add(new Label("Team:"));
        ToggleGroup teamGroup = new ToggleGroup();
        allyRadio = new RadioButton("Ally");
        allyRadio.setToggleGroup(teamGroup);
        allyRadio.setSelected(true);
        RadioButton enemyRadio = new RadioButton("Enemy");
        enemyRadio.setToggleGroup(teamGroup);
        teamBox.getChildren().addAll(allyRadio, enemyRadio);

        spawnedCheckBox = new CheckBox("Spawned");
        spawnedCheckBox.setSelected(true);

        HBox inventorBox = new HBox(8);
        inventorBox.getChildren().add(new Label("Inventory:"));
        inventorField = new TextField();
        inventorField.setPrefWidth(150);
        inventorBox.getChildren().add(inventorField);

        oreBoxRow = new HBox(8);
        oreBoxRow.getChildren().add(new Label("Ore:"));
        oreField = new TextField();
        oreField.setText("0");
        oreField.setPrefWidth(100);
        oreField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        oreBoxRow.getChildren().add(oreField);

        killsBox = new HBox(8);
        killsBox.getChildren().add(new Label("Kills:"));
        killsField = new TextField();
        killsField.setText("0");
        killsField.setPrefWidth(100);
        killsField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        killsBox.getChildren().add(killsField);



        HBox buttonBox = new HBox(10);
        buttonBox.setStyle("-fx-alignment: center;");
        Button okButton = new Button("OK");
        okButton.setPrefWidth(80);
        okButton.setOnAction(e -> handleOK());
        Button cancelButton = new Button("Cancel");
        cancelButton.setPrefWidth(80);
        cancelButton.setOnAction(e -> handleCancel());
        buttonBox.getChildren().addAll(okButton, cancelButton);

        root.getChildren().addAll(
                typeBox,
                healthBox,
                damageBox,
                teamBox,
                inventorBox,
                new Separator(),
                buttonBox
        );

        updateOreVisibility();
        root.getChildren().add(root.getChildren().indexOf(buttonBox), oreBoxRow);
        root.getChildren().add(root.getChildren().indexOf(buttonBox), killsBox);

        Scene scene = new Scene(root);
        stage.setScene(scene);
    }

    private void updateOreVisibility() {
        boolean warriorSelected = "Warrior".equals(unitTypeCombo.getValue());
        boolean centurioSelected = "Centurio".equals(unitTypeCombo.getValue());
        oreBoxRow.setManaged(warriorSelected);
        oreBoxRow.setVisible(warriorSelected);
        killsBox.setManaged(centurioSelected);
        killsBox.setVisible(centurioSelected);
    }

    private void handleOK() {
        try {
            String unitType = unitTypeCombo.getValue();
            int health = Integer.parseInt(healthField.getText().trim());
            int damage = Integer.parseInt(damageField.getText().trim());
            boolean isSpawned = spawnedCheckBox.isSelected();
            boolean team = allyRadio.isSelected();

            ArrayList<String> inventor;
            String inventorInput = inventorField.getText().trim();
            if (inventorInput.isEmpty()) {
                inventor = new ArrayList<>();
            } else {
                inventor = new ArrayList<>(Arrays.asList(inventorInput.split("\\s*,\\s*")));
            }

            int ore = 0;
            if ("Warrior".equals(unitType)) {
                ore = Integer.parseInt(oreField.getText().trim());
            }

            int kills = 0;
            if ("Centurio".equals(unitType)) {
                kills = Integer.parseInt(killsField.getText().trim());
            }

            switch (unitType) {
                case "Warrior":
                    result = new Warrior(health, isSpawned, team, damage, false, inventor, 100, 100);
                    ((Warrior) result).setOreCount(ore);
                    break;
                case "Centurio":
                    result = new Centurio(health, isSpawned, team, damage, false, inventor, 100, 100);
                    ((Centurio) result).setKillCount(kills);
                    break;
                case "Pretorio":
                    result = new Pretorio(health, isSpawned, team, damage, false, inventor, 100, 100);
                    break;
            } 

            confirmed = true;
            stage.close();

        } catch (NumberFormatException ex) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Invalid input");
            alert.setContentText("Health, Damage, and Ore must be numbers.");
            alert.showAndWait();
        }
    }

    private void handleCancel() {
        confirmed = false;
        result = null;
        stage.close();
    }

    public Unit showAndWait() {
        try {
            System.out.println("DEBUG: Showing UnitCreationDialog...");
            stage.showAndWait();
            System.out.println("DEBUG: Dialog closed. Confirmed: " + confirmed);
            return confirmed ? result : null;
        } catch (Exception e) {
            System.err.println("ERROR in showAndWait: " + e.getMessage());
            e.printStackTrace();
            return null;
        }
    }
}
