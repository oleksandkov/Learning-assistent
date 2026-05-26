package org.example.lab4;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class ParamsDlg {
    public static void display(double mx, double my) {
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Введіть параметри для нового студента:");

        Label label1 = new Label("Ім'я");
        TextField nameText= new TextField();

        Label label2 = new Label("Активний");
        CheckBox activeCheck = new CheckBox();
        activeCheck.setSelected(false);

        Label label3 = new Label("Курс:");
        ComboBox kursCombo = new ComboBox();
        kursCombo.getItems().addAll("1", "2", "3","4","5","6" );


        Label label4 = new Label("Здоров'я:");

        ToggleGroup tgr = new ToggleGroup();

        RadioButton rb1 = new RadioButton("10%");
        rb1.setUserData("10");
        rb1.setToggleGroup(tgr);
        rb1.setSelected(true);

        RadioButton rb2 = new RadioButton("25%");
        rb2.setUserData("25");
        rb2.setToggleGroup(tgr);

        RadioButton rb3 = new RadioButton("50%");
        rb3.setUserData("50");
        rb3.setToggleGroup(tgr);

        RadioButton rb4 = new RadioButton("75%");
        rb4.setUserData("75");
        rb4.setToggleGroup(tgr);

        RadioButton rb5 = new RadioButton("100%");
        rb5.setUserData("100");
        rb5.setToggleGroup(tgr);



//addNewStudent( String name, boolean isActive, int kurs, double health, double x, double y){
        Button okButton = new Button("OK");
        okButton.setFont(new Font(18));
        okButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {

                double health=100.0;

                if( tgr.getSelectedToggle() != null ) {
                    RadioButton selection = (RadioButton) tgr.getSelectedToggle();
                    String rez= selection.getUserData().toString();
                    health=Double.parseDouble(rez);
                }

                HelloApplication.addNewStudent( nameText.getText(), activeCheck.isSelected(),
                        Integer.parseInt(kursCombo.getValue().toString()), health, mx, my) ;
                window.close();
            }
        });



        VBox layout = new VBox(5);
        layout.getChildren().addAll(label1, nameText, label2, activeCheck, label3, kursCombo, label4);

        layout.getChildren().addAll(rb1, rb2, rb3, rb4, rb5);

        layout.getChildren().addAll(okButton);
        Scene scene = new Scene(layout, 500,500);
        window.setScene(scene);

        window.showAndWait();


    }
}

