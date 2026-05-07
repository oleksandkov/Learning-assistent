package org.example.lab4;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class StudentParamsDlg {
    public static void display() {
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Введіть параметри для нового студента:");

        Label nameLabel = new Label("Ім'я:");
        nameLabel.setFont(new Font(18));

        TextField nameText= new TextField();
        nameText.setFont(new Font(18));

        Label kursLabel = new Label("Курс:");
        kursLabel.setFont(new Font(18));

        ComboBox kursCombo = new ComboBox();
        kursCombo.getItems().addAll("1", "2", "3","4","5" );

        Label healthLabel = new Label("Здоров'я:");
        healthLabel.setFont(new Font(18));

        ToggleGroup tgr = new ToggleGroup();

        RadioButton rb1 = new RadioButton("10%");
        rb1.setUserData("10");
        rb1.setToggleGroup(tgr);
        rb1.setSelected(true);
        rb1.setFont(new Font(18));

        RadioButton rb2 = new RadioButton("25%");
        rb2.setUserData("25");
        rb2.setToggleGroup(tgr);
        rb2.setFont(new Font(18));

        RadioButton rb3 = new RadioButton("50%");
        rb3.setUserData("50");
        rb3.setToggleGroup(tgr);
        rb3.setFont(new Font(18));

        RadioButton rb4 = new RadioButton("75%");
        rb4.setUserData("75");
        rb4.setToggleGroup(tgr);
        rb4.setFont(new Font(18));

        RadioButton rb5 = new RadioButton("100%");
        rb5.setUserData("100");
        rb5.setToggleGroup(tgr);
        rb5.setFont(new Font(18));

        Label activeLabel = new Label("Активований:");
        activeLabel.setFont(new Font(18));

        CheckBox activeCheck = new CheckBox();
        activeCheck.setSelected(false);
        activeCheck.setFont(new Font(18));

        Label xLabel = new Label("X:");
        xLabel.setFont(new Font(18));

        TextField xText= new TextField();
        xText.setFont(new Font(18));

        Label yLabel = new Label("Y:");
        yLabel.setFont(new Font(18));

        TextField yText= new TextField();
        yText.setFont(new Font(18));

        Button okButton = new Button("OK");
        okButton.setFont(new Font(45));
        okButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {

                String name = nameText.getText();

                int kurs=0;

                try{
                    kurs=Integer.parseInt(kursCombo.getValue().toString());
                }
                catch(Exception e){
                    kurs=1;
                }

                double health=0.0;

                if( tgr.getSelectedToggle() != null ) {
                    RadioButton selection = (RadioButton) tgr.getSelectedToggle();
                    String rez= selection.getUserData().toString();
                    try{
                    health=Double.parseDouble(rez);
                    }
                    catch(Exception e){
                        health=100;
                    }
                }

                boolean activate =activeCheck.isSelected();


                double x;
                try {
                    x= Double.parseDouble(xText.getText());
                }
                catch(Exception e){
                    x=0.0;
                }


                double y;
                try {
                    y= Double.parseDouble(yText.getText());
                }
                catch(Exception e){
                    y=0.0;
                }

                HelloApplication.addNewStudent(
                name, kurs, health,
                activate, x, y );

                    window.close();
            }
        });


        VBox layout = new VBox(5);
        layout.getChildren().addAll(nameLabel,nameText,kursLabel,kursCombo,healthLabel);
        layout.getChildren().addAll(rb1,rb2,rb3,rb4,rb5);
        layout.getChildren().addAll(activeLabel, activeCheck, xLabel, xText, yLabel, yText);
        layout.getChildren().add(okButton);

        Scene scene = new Scene(layout, 500,650);
        window.setScene(scene);

        window.showAndWait();


    }
}
