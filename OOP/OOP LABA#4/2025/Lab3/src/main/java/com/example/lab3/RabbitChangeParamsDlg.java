package com.example.lab3;

import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.util.ArrayList;
public class RabbitChangeParamsDlg {

    public static void display(int rabbitIndex ) {
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Введіть параметри нового зайця!");
        window.setMinWidth(250);

        ArrayList<String> paramsTochange = HelloApplication.world.getParamsToChange( rabbitIndex );

        VBox layout = new VBox(11);
        //layout.getChildren().addAll(label, closeButton);
        layout.setAlignment(Pos.CENTER);
//---------------------------------------------------------
        Label nameLabel=new Label();
        nameLabel.setText("Name:");
        TextField nameText = new TextField();
        nameText.setText(paramsTochange.get(0));

        Label healthLabel=new Label();
        healthLabel.setText("Health:");
        TextField healthText = new TextField();
        healthText.setText(paramsTochange.get(1));

        Label xLabel=new Label();
        xLabel.setText("X:");
        TextField xText = new TextField();
        xText.setText(paramsTochange.get(2));

        Label yLabel=new Label();
        yLabel.setText("Y:");
        TextField yText = new TextField();
        yText.setText(paramsTochange.get(3));
//---------------------------------------------------------
        Button okButton=new Button("OK");
        okButton.setOnAction(e->{

            String sName= nameText.getText();
            String sHealth= healthText.getText();
            String sX = xText.getText();
            String sY = yText.getText();

            HelloApplication.world.changeRabbit(rabbitIndex, sName, sHealth, sX, sY);

            window.close(); });


        layout.getChildren().addAll(nameLabel, nameText, healthLabel, healthText, xLabel, xText, yLabel, yText, okButton);

        Scene scene=new Scene(layout,303,300);
        window.setScene(scene);
        window.showAndWait();

    }

}
