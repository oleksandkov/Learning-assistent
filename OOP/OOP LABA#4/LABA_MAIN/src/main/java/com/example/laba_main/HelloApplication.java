package com.example.laba_main;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import javafx.application.Application;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.stage.Stage;


public class HelloApplication extends Application {

    public static Group group;
    public static Scene scene;

    public static Image imgWarrior;
    public static ArrayList<Unit> warriors;

    private Map<KeyCode, Double> keysPresses = new HashMap<>();
    private Map<KeyCode, Boolean> keysPressed = new HashMap<>();

    // Speed
    public static double keyStepX = 1.0;
    public static double keyStepY = 1.0;



   



    @Override
    public void start(Stage stage) throws IOException {
        group = new Group();
        scene = new Scene(group, 1080, 700);

        // Setup key maps
        keysPresses.put(KeyCode.W, -keyStepY);
        keysPresses.put(KeyCode.S, keyStepY);
        keysPresses.put(KeyCode.A, -keyStepX);
        keysPresses.put(KeyCode.D, keyStepX);
        keysPresses.put(KeyCode.UP, -keyStepY);
        keysPresses.put(KeyCode.DOWN, keyStepY);
        keysPresses.put(KeyCode.LEFT, -keyStepX);
        keysPresses.put(KeyCode.RIGHT, keyStepX);

        
        scene.setOnKeyPressed(e -> keysPressed.put(e.getCode(), true));
        scene.setOnKeyReleased(e -> keysPressed.remove(e.getCode()));

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

        
        javafx.animation.AnimationTimer gameLoop = new javafx.animation.AnimationTimer() {
            @Override
            public void handle(long now) {
                double dx = 0, dy = 0;
                
                for (KeyCode code : keysPressed.keySet()) {
                    if (keysPresses.containsKey(code)) {
                        double step = keysPresses.get(code);
                        if (code == KeyCode.W || code == KeyCode.UP) {
                            dy += step;
                        } else if (code == KeyCode.S || code == KeyCode.DOWN) {
                            dy += step;
                        } else if (code == KeyCode.A || code == KeyCode.LEFT) {
                            dx += step;
                        } else if (code == KeyCode.D || code == KeyCode.RIGHT) {
                            dx += step;
                        }
                    }
                }
                
                
                for (Unit warrior : warriors) {
                    if (warrior.isActive()) {
                        warrior.move(dx, dy);
                    }
                }
            }
        };
        gameLoop.start();




        // Footer
       
        stage.setScene(scene);
        stage.show();
    }
}
