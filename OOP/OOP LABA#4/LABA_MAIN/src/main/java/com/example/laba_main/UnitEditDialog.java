package com.example.laba_main;

import java.util.ArrayList;
import java.util.Arrays;

import javafx.geometry.Insets;
import javafx.scene.Scene;
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


public class UnitEditDialog {
    private Stage stage;
    private Unit unit;
    private boolean confirmed = false;

    private TextField healthField;
    private TextField damageField;
    private TextField inventorField;
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
                teamBox,
                new Separator(),
                buttonBox
        );

        Scene scene = new Scene(root);
        stage.setScene(scene);
    }

    
    private void handleOK() {
        try {

            boolean oldTeam = unit.getTeam();

            if (!healthField.getText().trim().isEmpty()) {
                int health = Integer.parseInt(healthField.getText().trim());
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


            boolean newTeam = teamButton.isSelected();
            unit.setTeam(newTeam);


            if (oldTeam != newTeam) {
                unit.updateTeamMark();
                System.out.println("Unit team changed and mark updated!");
            }

            confirmed = true;
            System.out.println("Unit updated: " + unit.getClass().getSimpleName());
            unit.updateTeamMark();
            stage.close();
        } catch (NumberFormatException e) {
            System.err.println("Invalid number format: " + e.getMessage());
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
