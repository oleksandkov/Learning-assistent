package com.example.laba_main;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.MouseButton;
import javafx.stage.Stage;
import javafx.util.Duration;


public class HelloApplication extends Application {

    public static Group group;
    public static Scene scene;

    public static Image imgWarrior;
    public static Image imgCenturio;
    public static Image imgPretorio;
    public static Image imgBase;
    public static Image imgTower;
    public static Image imgSource;
    public static ArrayList<Warrior> warriors;
    public static ArrayList<Centurio> centurios;
    public static ArrayList<Pretorio> pretorios;
    public static ArrayList<Unit> units;

    private final Map<KeyCode, Double> keysPresses = new HashMap<>();
    private final Map<KeyCode, Boolean> keysPressed = new HashMap<>();
    private final Set<KeyCode> handledActionKeys = new HashSet<>();

    // Speed
    public static double keyStepX = 3.0;
    public static double keyStepY = 3.0;

    // Number buildings 

    public static ArrayList<Tower> towersA = new ArrayList<>();
    public static ArrayList<Source> sourcesA = new ArrayList<>();
    public static ArrayList<Base> basesA = new ArrayList<>();
    public static ArrayList<Tower> towersB = new ArrayList<>();
    public static ArrayList<Source> sourcesB = new ArrayList<>();
    public static ArrayList<Base> basesB = new ArrayList<>();
    
    public static ArrayList<World> buldings = new ArrayList<>();

    private Label oreLabelTeamA;
    private Label oreLabelTeamB;
    private UnitInvetorWindow unitInvetorWindow;



   
    


    @Override
    public void start(Stage stage) throws IOException {
        group = new Group();
        scene = new Scene(group, 1080, 700);
        unitInvetorWindow = new UnitInvetorWindow();

        oreLabelTeamA = new Label();
        oreLabelTeamB = new Label();
        oreLabelTeamA.setLayoutX(0);
        oreLabelTeamA.setLayoutY(0);
        oreLabelTeamB.setLayoutX(0);
        oreLabelTeamB.setLayoutY(20);
        group.getChildren().addAll(oreLabelTeamA, oreLabelTeamB);

        // Setup key maps
        keysPresses.put(KeyCode.W, -keyStepY);
        keysPresses.put(KeyCode.S, keyStepY);
        keysPresses.put(KeyCode.A, -keyStepX);
        keysPresses.put(KeyCode.D, keyStepX);
        keysPresses.put(KeyCode.UP, -keyStepY);
        keysPresses.put(KeyCode.DOWN, keyStepY);
        keysPresses.put(KeyCode.LEFT, -keyStepX);
        keysPresses.put(KeyCode.RIGHT, keyStepX);

        scene.setOnKeyPressed(e -> {
            KeyCode code = e.getCode();
            if (code == KeyCode.DELETE || code == KeyCode.INSERT || code == KeyCode.ESCAPE || code == KeyCode.SPACE || code == KeyCode.I) {
                if (handledActionKeys.contains(code)) {
                    return;
                }
                handledActionKeys.add(code);
            }
            if (code == KeyCode.V && e.isControlDown()) {
                if (handledActionKeys.contains(code)) {
                    return;
                }
                handledActionKeys.add(code);
            }
            keysPressed.put(code, true);
        });
        scene.setOnKeyReleased(e -> {
            KeyCode code = e.getCode();
            keysPressed.remove(code);
            handledActionKeys.remove(code);
        });


        URL warriorUrl = HelloApplication.class.getResource("/warrior.png");
        URL CenturioUrl = HelloApplication.class.getResource("/centurio.png");
        URL PretorioUrl = HelloApplication.class.getResource("/pretorio.png");
        URL baseUrl = HelloApplication.class.getResource("/base2.png");
        URL towerUrl = HelloApplication.class.getResource("/tower2.png");
        URL sourceUrl = HelloApplication.class.getResource("/source2.png");
        if (warriorUrl == null) {
            throw new IllegalStateException("Resource not found: /warrior.png");
        }
        if (CenturioUrl == null) {
            throw new IllegalStateException("Resource not found: /centurio.png");
        }
        if (PretorioUrl == null) {
            throw new IllegalStateException("Resource not found: /pretorio.png");
        }
        if (baseUrl == null) {
            throw new IllegalStateException("Resource not found: /base.png");
        }
        if (towerUrl == null) {
            throw new IllegalStateException("Resource not found: /tower.png");
        }
        if (sourceUrl == null) {
            throw new IllegalStateException("Resource not found: /source.png");
        }

        imgBase = new Image(baseUrl.toExternalForm(), 250, 250, false, false);
        imgTower = new Image(towerUrl.toExternalForm(), 200, 200, false, false);
        imgSource = new Image(sourceUrl.toExternalForm(),200, 200, false, false);
        imgWarrior = new Image(warriorUrl.toExternalForm(), 100, 100, false, false);
        imgCenturio = new Image(CenturioUrl.toExternalForm(), 100, 100, false, false);
        imgPretorio = new Image(PretorioUrl.toExternalForm(), 100, 100, false, false);
        warriors = new ArrayList<>();
        centurios = new ArrayList<>();
        pretorios = new ArrayList<>();
        units = new ArrayList<>();

        // warriors.add(new Warrior(100, true, true, 7, false,
        //         new ArrayList<>(Arrays.asList("Knife", "Shield")), 80, 80));
        // warriors.add(new Warrior(100, true, false, 9, false,
        //         new ArrayList<>(Arrays.asList("Axe")), 260, 120));
        // warriors.add(new Warrior(100, true, true, 6, false,
        //         new ArrayList<>(Arrays.asList("Sword", "Potion")), 460, 220));
        // pretorios.add(new Pretorio(150, true, true, 11, false, new ArrayList<>(Arrays.asList("Shield")), 120, 80));
        // centurios.add(new Centurio(120, true, true, 10, false, new ArrayList<>(Arrays.asList("Shield")), 110, 110));
        for (int i = 0; i < warriors.size(); i++) {
            Unit u = warriors.get(i);
            units.add(u);
        }

        for (Unit warrior : warriors) {
            warrior.resurrect();
        }
        for  (int i = 0; i < centurios.size(); i++) {
            Unit c = centurios.get(i);
            units.add(c);
        }
        for (Unit centurios  : centurios) {
            centurios.resurrect();
        }
        for (int i  = 0; i < pretorios.size(); i++) {
            Unit p =  pretorios.get(i);
            units.add(p);
        }
        for (Unit p : pretorios) {
            p.resurrect();
        }
       



        // Towers
        Tower tower1 = new Tower();
        tower1.setTeam(true);
        tower1.initGraphics(imgTower, "Tower", 0, 100,0, 300.0, 50);
        tower1.resurrectWorld();
        Timeline healTimer = new Timeline(
            new KeyFrame(Duration.seconds(1), e -> tower1.healUnits())
        );
        healTimer.setCycleCount(Animation.INDEFINITE);
        healTimer.play();

        Tower tower2 = new Tower();
        tower2.setTeam(false);
        tower2.initGraphics(imgTower, "Tower", 0, 600,0, 300.0, 300);
        tower2.resurrectWorld();
        Timeline healTimer2 = new Timeline(
            new KeyFrame(Duration.seconds(1), e -> tower2.healUnits())
        );
        healTimer2.setCycleCount(Animation.INDEFINITE);
        healTimer2.play();

        towersB.add(tower2);
        towersA.add(tower1);
        buldings.add(tower1);
        buldings.add(tower2);

        // Sources
        Source source1 = new Source();
        source1.setTeam(true);
        source1.initGraphics(imgSource, "Source", 0, 100, 300, 200.0, 200);
        source1.resurrectWorld();
        Source source2 = new Source();
        source2.setTeam(false);
        source2.initGraphics(imgSource, "Source", 0, 600, 300, 200.0, 200);
        source2.resurrectWorld();
        sourcesB.add(source2);
        sourcesA.add(source1);
        buldings.add(source1);
        buldings.add(source2);

        // Bases
        Base base1 = new Base();
        base1.setTeam(true);
        base1.initGraphics(imgBase, "Base", 0, 100, 650, 200.0, 200);
        base1.resurrectWorld();
        Base base2 = new Base();
        base2.setTeam(false);
        base2.initGraphics(imgBase, "Base", 0, 600, 650, 200.0, 200);
        base2.resurrectWorld();
        basesB.add(base2);
        basesA.add(base1);
        buldings.add(base1);
        buldings.add(base2);


        
         World world = new World(units);


        scene.setOnMouseClicked(event -> {
            if (event.getButton() == MouseButton.PRIMARY) {
                
                for (int i = units.size() - 1; i >= 0; i--) {
                    Unit unit = units.get(i);
                    if (unit.tryActivate(event.getX(), event.getY())) {
                        break;
                    }
                }
            } else if (event.getButton() == MouseButton.SECONDARY) {
               
                for (int i = units.size() - 1; i >= 0; i--) {
                    Unit unit = units.get(i);
                    if (unit.getImage().getBoundsInParent().contains(event.getX(), event.getY())) {
                        javafx.application.Platform.runLater(() -> {
                            UnitEditDialog editDialog = new UnitEditDialog(unit);
                            editDialog.showAndWait();
                        });
                        break;
                    }
                }
            }
        });

        
        javafx.animation.AnimationTimer gameLoop = new javafx.animation.AnimationTimer() {
            @Override
            public void handle(long now) {
                double dx = 0, dy = 0;

                for (KeyCode code : new ArrayList<>(keysPressed.keySet())) {
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
                    } else if (code == KeyCode.DELETE) {
                        for (int i = units.size() - 1; i >= 0; i--) {
                            for (Unit unit : units) {
                                if (unit.isActive()) {
                                    unit.removeUnitFromGame();
                                    break;
                                }
                            }
                        }
                        keysPressed.remove(KeyCode.DELETE);
                    } else if (code == KeyCode.INSERT) {
                        
                        
                        javafx.application.Platform.runLater(() -> {
                              
                                UnitCreationDialog dialog = new UnitCreationDialog();
                                Unit newUnit = dialog.showAndWait();
                                double x;
                                double y;
                                if (newUnit != null) {
                                    units.add(newUnit);

                                    boolean team = newUnit.getTeam();
                                    if (team) {
                                        x = basesA.get(0).x;
                                        y = basesA.get(0).y;
                                    } else {
                                        x = basesB.get(0).x;
                                        y = basesB.get(0).y;
                                    }
                                    newUnit.setPosition(x, y);
                                    newUnit.resurrect();
                                } else {
                                    System.out.println("Dialog was cancelled or null result");
                                }
                            
                        });
                        keysPressed.remove(KeyCode.INSERT);
                    } else if (code == KeyCode.ESCAPE) {
                        for (int i = 0; i < units.size(); i++) {
                            for (Unit unit : units) {
                                if (unit.isActive()) {
                                    unit.flipActivation();
                                }
                            }
                        }
                        keysPressed.remove(KeyCode.ESCAPE);
                    } else if (code == KeyCode.V) {
                        ArrayList<Unit> clones = new ArrayList<>();
                        int cloneIndex = 0;
                        for (Unit unit : units) {
                            if (unit.isActive()) {
                                try {
                                    Unit clonedUnit = (Unit) unit.clone();
                                    int offset = 20 + (cloneIndex * 10);
                                    clonedUnit.setPosition(unit.x + offset, unit.y + offset);
                                    clones.add(clonedUnit);
                                    cloneIndex++;
                                } catch (CloneNotSupportedException e) {
                                    System.err.println("Clone not supported: " + e.getMessage());
                                    e.printStackTrace();
                                }
                            }
                        }
                        for (Unit clonedUnit : clones) {
                            units.add(clonedUnit);
                            clonedUnit.resurrect();
                        }
                        keysPressed.remove(KeyCode.V);
                    } else if (code == KeyCode.SPACE) {
                        for (Unit unit : units) {
                            if (unit.isActive()) {
                                unit.attack();
                            }
                        }
                        keysPressed.remove(KeyCode.SPACE);
                    } else if (code == KeyCode.I) {
                        if (unitInvetorWindow.isVisible()) {
                            unitInvetorWindow.setVisible(false);
                        } else {
                            unitInvetorWindow.showForActiveUnit(units);
                        }
                        keysPressed.remove(KeyCode.I);
                    }
                }

                for (World world : buldings) {
                    if (world.getHealth() <= 0) {
                        world.removeBuildingFromGame();
                    }
                }

                updateOreHud();

                tower1.intersect();
                tower2.intersect();
                source1.intersect();
                source2.intersect();
                base1.intersect();
                base2.intersect();
                
                for (Unit unit : units) {
                    if (unit.isActive()) {
                        unit.move(dx, dy);
                    }
                    unit.logic();
                }

                unitInvetorWindow.updateFromUnits(units);
                
            }
        };
        gameLoop.start();




        // Footer
       
        stage.setScene(scene);
        stage.show();
    }

    private void updateOreHud() {
        double teamAOre = 0;
        double teamBOre = 0;

        for (Base base : basesA) {
            if (base != null) {
                teamAOre += base.getOre();
            }
        }

        for (Base base : basesB) {
            if (base != null) {
                teamBOre += base.getOre();
            }
        }
            oreLabelTeamA.setText("Team A ore: " + (int) teamAOre);
            oreLabelTeamB.setText("Team B ore: " + (int) teamBOre);
        
    }

}
