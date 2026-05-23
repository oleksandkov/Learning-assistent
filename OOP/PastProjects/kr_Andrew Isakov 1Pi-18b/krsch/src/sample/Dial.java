package sample;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.VBox;
import javafx.scene.shape.Circle;
import javafx.scene.text.Font;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.util.Random;

public class Dial {

    static boolean answer = false;

    static boolean side ;
    static boolean active = false;

    public static double x = 0;
    public static double y = 0;

    static Circle r;
    static Image i;
    static ImageView iv;
    static Label l;

    static double speed;

    static int c;
    public static boolean dial() {
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);//Block events to other windows
        window.setTitle("Deus ex machina mode");
        // coords X and Y
        Label label1 = new Label("X:");
        label1.setFont(new Font(20));
        TextField textField1 = new TextField();
        textField1.setFont(new Font(20));

        Label label2 = new Label("Y:");
        label2.setFont(new Font(20));
        TextField textField2 = new TextField();
        textField2.setFont(new Font(20));

        Label label_speed = new Label("Speed:");
        label_speed.setFont(new Font(20));
        TextField textField_speed = new TextField();
        textField_speed.setFont(new Font(20));
        // Side
        ToggleGroup group = new ToggleGroup();
        Label label = new Label("Side:");
        label.setFont(new Font(20));
        RadioButton good = new RadioButton("Good");
        good.setToggleGroup(group);

        RadioButton bad = new RadioButton("Bad");
        bad.setToggleGroup(group);

        RadioButton rand = new RadioButton("Rand");
        rand.setToggleGroup(group);
        good.setSelected(true);

        // checkbox for control
        CheckBox ctrl = new CheckBox("Active?");
        //ctrl.setSelected(true);

        // level
        ComboBox comboBox = new ComboBox<>();
        comboBox.getItems().addAll(
                "1. Mario",
                "2. Yoshi",
                "3. Princess"
               // "4. Bad"
        );

        comboBox.setPromptText("Level");
        Button submit = new Button("Submit");
        submit.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                answer = true;
                x = Integer.parseInt(textField1.getText());
                y = Integer.parseInt(textField2.getText());
                speed = Double.parseDouble(textField_speed.getText());
                if (good.isSelected()) {
                    side = true;

                }
                if (bad.isSelected()) {
                    side = false;
                }
                if (rand.isSelected()) {
                    Random random = new Random();
                    side = random.nextBoolean();
                }
                if (ctrl.isSelected()) {
                    active = true;
                }
                if (comboBox.getValue() == "1. Mario") c=1;
                if (comboBox.getValue() == "2. Yoshi")c=2;
                if (comboBox.getValue() =="3. Princess")c=3;
                //if (comboBox.getValue() =="4. Bad")c=4;

 /*if(X.isEmpty() || Y.isEmpty() || spee.isEmpty()) {
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.initOwner(new Stage());
                    alert.initModality(Modality.APPLICATION_MODAL);
                    alert.setContentText("Please fill all fields");
                    alert.showAndWait();
                }
 */
                window.close();
            }
        });
        //ComboBoxes also generate actions if you need to get value instantly
        //comboBox.setOnAction( e -> System.out.println("User selected " + comboBox.getValue()) );
        comboBox.setPrefSize(100,50);
        submit.setPrefSize(100,50);
        VBox layout = new VBox(10);
        layout.getChildren().addAll(
                label1, textField1,
                label2, textField2,
                label_speed, textField_speed,
                comboBox,
                label,
                good,
                bad,
                rand,
                ctrl,
                submit
        );
        Scene scene = new Scene(layout, 400, 600);
        window.setScene(scene);
        window.showAndWait();
        return answer;
    }
}


