package org.example.lab4;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.util.ArrayList;

public class StudentChangeParamsDlg {
    public static void display(Student student) {
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Відредагуйте параметри студента:");

        Label nameLabel = new Label("Ім'я:");
        nameLabel.setFont(new Font(18));

        TextField nameText= new TextField();
        nameText.setFont(new Font(18));
        nameText.setText(student.getName());

        Label kursLabel = new Label("Курс:");
        kursLabel.setFont(new Font(18));

        ComboBox kursCombo = new ComboBox();
        kursCombo.getItems().addAll("1", "2", "3","4","5" );

        int kurs= student.getKurs();
        String strKurs= Integer.toString(kurs) ;

        if( (kurs>0) && (kurs<6) ){
            kursCombo.setValue(strKurs);
        }
        else{
            kursCombo.getItems().addAll(strKurs);
            kursCombo.setValue(strKurs);
        }

        Label healthLabel = new Label("Здоров'я:");
        healthLabel.setFont(new Font(18));

        TextField healthText= new TextField();
        healthText.setFont(new Font(18));
        double health = student.getHealth();
        healthText.setText(Double.toString(health));

        Label activeLabel = new Label("Активований:");
        activeLabel.setFont(new Font(18));

        CheckBox activeCheck = new CheckBox();
        if(student.isActive()) activeCheck.setSelected(true);
        else activeCheck.setSelected(false);
        activeCheck.setFont(new Font(18));


        Label xLabel = new Label("X:");
        xLabel.setFont(new Font(18));

        TextField xText= new TextField();
        xText.setFont(new Font(18));
        double x= student.getX();
        xText.setText(Double.toString(x));

        Label yLabel = new Label("Y:");
        yLabel.setFont(new Font(18));

        TextField yText= new TextField();
        yText.setFont(new Font(18));
        double y= student.getY();
        yText.setText(Double.toString(y));

        ComboBox examsCombo = new ComboBox();
        ArrayList<String> examsStr=  student.getZachotkaContents();
        for( String s:examsStr )
            examsCombo.getItems().add(s);

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

                double health;
                try{
                    health =Double.parseDouble(healthText.getText());
                }
                catch(Exception e){
                    health=100;
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

                HelloApplication.adjustStudent(
                        student, name, kurs, health,
                        activate, x, y );
                window.close();
            }
        });
//----------------------------------------------------------------

        VBox layout = new VBox(5);
        layout.getChildren().addAll(nameLabel,nameText,kursLabel,kursCombo,healthLabel,healthText);
        layout.getChildren().addAll(activeLabel, activeCheck, xLabel, xText, yLabel, yText,examsCombo);
        layout.getChildren().add(okButton);

        Scene scene = new Scene(layout, 500,650);
        window.setScene(scene);

        window.showAndWait();

    }
}
