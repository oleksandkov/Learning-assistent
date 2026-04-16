package com.example.laba_main;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;

import javafx.application.Application;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

public class HelloApplication extends Application {

    public static Group group;
    public static Scene scene;

    public static Image imgWarrior;
    public static ArrayList<Unit> warriors;

    @Override
    public void start(Stage stage) throws IOException {
//        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
//        Scene scene = new Scene(fxmlLoader.load(), 320, 240);

        group = new Group();
        scene = new Scene(group, 1080, 700);

        URL warriorUrl = HelloApplication.class.getResource("/warrior_backup_before_transparency-removebg-preview.png");
        if (warriorUrl == null) {
            throw new IllegalStateException("Resource not found: /warrior_backup_before_transparency-removebg-preview.png");
        }
        imgWarrior = new Image(warriorUrl.toExternalForm(), 100, 100, false, false);

        warriors = new ArrayList<>();

        warriors.add(new Warrior(100, true, "ally", 7, false,
                new ArrayList<>(Arrays.asList("Knife", "Shield")), 80, 80));
        warriors.add(new Warrior(100, true, "enemy", 9, false,
                new ArrayList<>(Arrays.asList("Axe")), 260, 120));
        warriors.add(new Warrior(100, true, "ally", 6, false,
                new ArrayList<>(Arrays.asList("Sword", "Potion")), 460, 220));

        for (Unit warrior : warriors) {
            warrior.resurrect();
        }

        scene.setOnMouseClicked(event -> {
            for (int i = warriors.size() - 1; i >= 0; i--) {
                Unit warrior = warriors.get(i);
                if (warrior.tryActivate(event.getX(), event.getY())) {
                    break;
                }
            }
        });




        // Footer
       
        stage.setScene(scene);
        stage.show();
    }
}
