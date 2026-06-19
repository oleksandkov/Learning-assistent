package org.example.laba5;

import java.util.ArrayList;
import java.util.Arrays;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Separator;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;
import org.example.laba5.Unit.Unit;
import org.example.laba5.Unit.Warrior;
import org.example.laba5.Unit.Centurio;

public class UnitEditDialog {
    private Stage stage;
    private final Unit unit;
    private boolean confirmed = false;

    private TextField healthField;
    private TextField damageField;
    private TextField inventorField;
    private TextField oreField;
    private HBox oreBox;
    private TextField killsField;
    private HBox killsBox;
    private RadioButton teamButton;

    public UnitEditDialog(Unit unit) {
        this.unit = unit;
        createDialog();
    }

    private void createDialog() {
        stage = new Stage();
        stage.initModality(Modality.APPLICATION_MODAL);
        stage.setTitle("Edit Unit - " + unit.getClass().getSimpleName());
        stage.setWidth(350);
        stage.setHeight(350);
        stage.setResizable(false);

        VBox root = new VBox(10);
        root.setPadding(new Insets(15));

        Label titleLabel = new Label("Unit Type: " + unit.getClass().getSimpleName());

        HBox healthBox = new HBox(10);
        Label healthLabel = new Label("Health:");
        healthLabel.setPrefWidth(80);
        healthField = new TextField();
        healthField.setText(String.valueOf(unit.getHealth()));
        healthField.setPrefWidth(150);
        healthField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        healthBox.getChildren().addAll(healthLabel, healthField);

        HBox damageBox = new HBox(10);
        Label damageLabel = new Label("Damage:");
        damageLabel.setPrefWidth(80);
        damageField = new TextField();
        damageField.setText(String.valueOf(unit.getDamage()));
        damageField.setPrefWidth(150);
        damageField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        damageBox.getChildren().addAll(damageLabel, damageField);

        HBox inventorBox = new HBox(10);
        Label inventorLabel = new Label("Inventory:");
        inventorLabel.setPrefWidth(80);
        inventorField = new TextField();
        ArrayList<String> inventor = unit.getInventor();
        if (inventor != null && !inventor.isEmpty()) {
            inventorField.setText(String.join(", ", inventor));
        }
        inventorField.setPrefWidth(200);
        inventorBox.getChildren().addAll(inventorLabel, inventorField);

        oreBox = new HBox(10);
        Label oreLabel = new Label("Ore:");
        oreLabel.setPrefWidth(80);
        oreField = new TextField();
        oreField.setPrefWidth(150);
        oreField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        if (unit.getClass() == Warrior.class) {
            Warrior warrior = (Warrior) unit;
            oreField.setText(String.valueOf((int) warrior.getOre()));
        } else {
            oreField.setText("N/A");
            oreField.setEditable(false);
        }
        oreBox.getChildren().addAll(oreLabel, oreField);

        killsBox = new HBox(10);
        Label killsLabel = new Label("Kills:");
        killsLabel.setPrefWidth(80);
        killsField = new TextField();
        killsField.setPrefWidth(150);
        killsField.setTextFormatter(new TextFormatter<>(c -> {
            if (c.getControlNewText().matches("\\d*")) return c;
            return null;
        }));
        if (unit.getClass() == Centurio.class) {
            Centurio centurio = (Centurio) unit;
            killsField.setText(String.valueOf(centurio.getKillCount()));
        } else {
            killsField.setText("N/A");
            killsField.setEditable(false);
        }
        killsBox.getChildren().addAll(killsLabel, killsField);

        HBox teamBox = new HBox(10);
        Label teamLabel = new Label("Team:");
        teamLabel.setPrefWidth(80);
        teamButton = new RadioButton("Ally");
        teamButton.setSelected(unit.getTeam());

        teamButton.setPrefWidth(150);
        teamBox.getChildren().addAll(teamLabel, teamButton);

        HBox buttonBox = new HBox(10);
        buttonBox.setStyle("-fx-alignment: center;");

        Button okButton = new Button("Apply");
        okButton.setPrefWidth(80);
        okButton.setStyle("-fx-font-size: 12;");
        okButton.setOnAction(e -> handleOK());

        Button cancelButton = new Button("Cancel");
        cancelButton.setPrefWidth(80);
        cancelButton.setStyle("-fx-font-size: 12;");
        cancelButton.setOnAction(e -> handleCancel());
        
        buttonBox.getChildren().addAll(okButton, cancelButton);

        root.getChildren().addAll(
                titleLabel,
                new Separator(),
                healthBox,
                damageBox,
                inventorBox,
                oreBox,
                killsBox,
                teamBox,
                new Separator(),
                buttonBox
        );

        Scene scene = new Scene(root);
        root.setStyle("-fx-background-color: #37ea23; -fx-border-color: #000000; -fx-border-width: 10px;");
        stage.setScene(scene);
    }

    private void handleOK() {
        try {
            boolean oldTeam = unit.getTeam();

            String healthParsed = healthField.getText().trim();
            if (!healthParsed.isEmpty()) {
                int health = Integer.parseInt(healthParsed);
                if (health < 1 || health > 250) {
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.setTitle("Error");
                    alert.setHeaderText("Invalid health");
                    alert.setContentText("Health must be between 1 and 250.");
                    alert.showAndWait();
                    return;
                }
                unit.setHealth(health);
                unit.setBaseHealth(health);
            }

            if (!damageField.getText().trim().isEmpty()) {
                int damage = Integer.parseInt(damageField.getText().trim());
                unit.setDamage(damage);
                unit.setBaseDamage(damage);
            }

            String inventorInput = inventorField.getText().trim();
            ArrayList<String> inventor;
            if (inventorInput.isEmpty()) {
                inventor = new ArrayList<>();
            } else {
                inventor = new ArrayList<>(Arrays.asList(inventorInput.split("\\s*,\\s*")));
            }
            unit.setInventor(inventor);

            if (unit.getClass() == Warrior.class) {
                Warrior warrior = (Warrior) unit;
                int ore = Integer.parseInt(oreField.getText().trim());
                warrior.setOreCount(ore);
            }

            if (unit.getClass() == Centurio.class) {
                Centurio centurio = (Centurio) unit;
                int kills = Integer.parseInt(killsField.getText().trim());
                centurio.setKillCount(kills);
            }

            boolean newTeam = teamButton.isSelected();
            unit.setTeam(newTeam);

            if (oldTeam != newTeam) {
                unit.updateTeamMark();
            }

            confirmed = true;
            unit.updateTeamMark();
            stage.close();
        } catch (NumberFormatException e) {
        }
    }

    private void handleCancel() {
        confirmed = false;
        stage.close();
    }

    public boolean showAndWait() {
        stage.showAndWait();
        return confirmed;
    }
}
