package org.example.laba5;

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
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;
import javafx.util.Duration;


public class HelloApplication extends Application {

    public static final double WORLD_WIDTH = 6400.0;
    public static final double WORLD_HEIGHT = 4800.0;
    private static final double VIEWPORT_WIDTH = 1920.0;
    private static final double VIEWPORT_HEIGHT = 1080.0;

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

    public static double keyStepX = 4.5;
    public static double keyStepY = 4.5;

    public static ArrayList<Tower> towersA = new ArrayList<>();
    public static ArrayList<Source> sourcesA = new ArrayList<>();
    public static ArrayList<Base> basesA = new ArrayList<>();
    public static ArrayList<Tower> towersB = new ArrayList<>();
    public static ArrayList<Source> sourcesB = new ArrayList<>();
    public static ArrayList<Base> basesB = new ArrayList<>();
    
    public static ArrayList<World> buldings = new ArrayList<>();
    private Pane cameraViewport;
    private Pane worldLayer;
    private Pane overlayPane;
    private Label oreLabelTeamA;
    private Label oreLabelTeamB;
    private UnitInvetorWindow unitInvetorWindow;
    private UnitSearchWindow unitSearchWindow;
    private MiniMapOverlay miniMapOverlay;
    private double cameraX;
    private double cameraY;
    public static Label numUnitsTeamA;
    public static Label numUnitsTeamB;

    @Override
    public void start(Stage stage) throws IOException {
        group = new Group();
        worldLayer = new Pane();
        worldLayer.setPrefSize(WORLD_WIDTH, WORLD_HEIGHT);
        Rectangle worldBounds = new Rectangle(WORLD_WIDTH, WORLD_HEIGHT);
        worldBounds.setFill(Color.TRANSPARENT);
        worldLayer.getChildren().add(worldBounds);
        group.getChildren().add(worldLayer);

        cameraViewport = new Pane();
        cameraViewport.setPrefSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        cameraViewport.setMinSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        cameraViewport.setMaxSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        cameraViewport.setStyle("-fx-background-color: transparent;");
        cameraViewport.setClip(new Rectangle(VIEWPORT_WIDTH, VIEWPORT_HEIGHT));
        cameraViewport.getChildren().add(group);

        overlayPane = new Pane();
        overlayPane.setPickOnBounds(false);

        Pane root = new Pane(cameraViewport, overlayPane);
        scene = new Scene(root, 1920, 1080);
        unitInvetorWindow = new UnitInvetorWindow();
        unitSearchWindow = new UnitSearchWindow();

        oreLabelTeamA = new Label();
        oreLabelTeamB = new Label();
        numUnitsTeamA = new Label();
        numUnitsTeamB = new Label();
        oreLabelTeamA.setLayoutX(0);
        oreLabelTeamA.setLayoutY(0);
        oreLabelTeamB.setLayoutX(0);
        oreLabelTeamB.setLayoutY(20);
        numUnitsTeamA.setLayoutX(0);
        numUnitsTeamA.setLayoutY(40);
        numUnitsTeamB.setLayoutX(0);
        numUnitsTeamB.setLayoutY(60);
        overlayPane.getChildren().addAll(oreLabelTeamA, oreLabelTeamB, numUnitsTeamA, numUnitsTeamB);
        keysPresses.put(KeyCode.W, -keyStepY);
        keysPresses.put(KeyCode.S, keyStepY);
        keysPresses.put(KeyCode.A, -keyStepX);
        keysPresses.put(KeyCode.D, keyStepX);
        keysPresses.put(KeyCode.UP, -keyStepY);
        keysPresses.put(KeyCode.DOWN, keyStepY);
        keysPresses.put(KeyCode.LEFT, -keyStepX);
        keysPresses.put(KeyCode.RIGHT, keyStepX);

        cameraX = 0.0;
        cameraY = 0.0;

        scene.setOnKeyPressed(e -> {
            KeyCode code = e.getCode();
            if (code == KeyCode.M) {
                if (miniMapOverlay != null) {
                    miniMapOverlay.toggleVisible();
                }
                return;
            }
            if (code == KeyCode.V && e.isControlDown()) {
                if (handledActionKeys.contains(KeyCode.V)) {
                    return;
                }
                handledActionKeys.add(KeyCode.V);
                cloneActiveUnit();
                return;
            }

            if (code == KeyCode.DELETE || code == KeyCode.INSERT || code == KeyCode.ESCAPE || code == KeyCode.SPACE || code == KeyCode.I || code == KeyCode.V || code == KeyCode.F) {
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

        Warrior warrior1 = new Warrior();
        warrior1.setTeam(true);
        warriors.add(warrior1);
        Warrior warrior2 = new Warrior();
        warrior2.setTeam(false);
        warriors.add(warrior2);
        Warrior warrior3 = new Warrior();
        warrior3.setTeam(true);
        warriors.add(warrior3);
        Warrior warrior4 = new Warrior();
        warrior4.setTeam(false);
        warriors.add(warrior4);

        for (int i = 0; i < warriors.size(); i++) {
            Unit u = warriors.get(i);
            units.add(u);
        }

        for  (int i = 0; i < centurios.size(); i++) {
            Unit c = centurios.get(i);
            units.add(c);
        }
        for (int i  = 0; i < pretorios.size(); i++) {
            Unit p =  pretorios.get(i);
            units.add(p);
        }

        // Team A (Ally - True)
        Base base1 = new Base();
        base1.setTeam(true);
        base1.initGraphics(imgBase, "base1", 0, 400, 400, 200.0, 200);
        base1.resurrectWorld();
        basesA.add(base1);
        buldings.add(base1);

        Source source1 = new Source();
        source1.setTeam(true);
        source1.initGraphics(imgSource, "source1", 0, 200, 1000, 200.0, 200);
        source1.resurrectWorld();
        sourcesA.add(source1);
        buldings.add(source1);

        Source source3 = new Source();
        source3.setTeam(true);
        source3.initGraphics(imgSource, "source3", 0, 1000, 200, 200.0, 200);
        source3.resurrectWorld();
        sourcesA.add(source3);
        buldings.add(source3);

        Tower tower1 = new Tower();
        tower1.setTeam(true);
        tower1.initGraphics(imgTower, "tower1", 0, 2500, 2000, 300.0, 300);
        tower1.resurrectWorld();
        towersA.add(tower1);
        buldings.add(tower1);
        Timeline healTimer1 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower1.healUnits()));
        healTimer1.setCycleCount(Animation.INDEFINITE);
        healTimer1.play();

        Tower tower3 = new Tower();
        tower3.setTeam(true);
        tower3.initGraphics(imgTower, "tower3", 0, 1800, 2300, 300.0, 300);
        tower3.resurrectWorld();
        towersA.add(tower3);
        buldings.add(tower3);
        Timeline healTimer3 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower3.healUnits()));
        healTimer3.setCycleCount(Animation.INDEFINITE);
        healTimer3.play();

        Tower tower5 = new Tower();
        tower5.setTeam(true);
        tower5.initGraphics(imgTower, "tower5", 0, 2700, 1300, 300.0, 300);
        tower5.resurrectWorld();
        towersA.add(tower5);
        buldings.add(tower5);
        Timeline healTimer5 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower5.healUnits()));
        healTimer5.setCycleCount(Animation.INDEFINITE);
        healTimer5.play();

        // Team B (Enemy - False)
        Base base2 = new Base();
        base2.setTeam(false);
        base2.initGraphics(imgBase, "base2", 0, 5750, 4150, 200.0, 200);
        base2.resurrectWorld();
        basesB.add(base2);
        buldings.add(base2);

        Source source2 = new Source();
        source2.setTeam(false);
        source2.initGraphics(imgSource, "source2", 0, 6000, 3600, 200.0, 200);
        source2.resurrectWorld();
        sourcesB.add(source2);
        buldings.add(source2);

        Source source4 = new Source();
        source4.setTeam(false);
        source4.initGraphics(imgSource, "source4", 0, 5200, 4400, 200.0, 200);
        source4.resurrectWorld();
        sourcesB.add(source4);
        buldings.add(source4);

        Tower tower2 = new Tower();
        tower2.setTeam(false);
        tower2.initGraphics(imgTower, "tower2", 0, 3700, 2600, 300.0, 300);
        tower2.resurrectWorld();
        towersB.add(tower2);
        buldings.add(tower2);
        Timeline healTimer2 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower2.healUnits()));
        healTimer2.setCycleCount(Animation.INDEFINITE);
        healTimer2.play();

        Tower tower4 = new Tower();
        tower4.setTeam(false);
        tower4.initGraphics(imgTower, "tower4", 0, 4400, 2300, 300.0, 300);
        tower4.resurrectWorld();
        towersB.add(tower4);
        buldings.add(tower4);
        Timeline healTimer4 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower4.healUnits()));
        healTimer4.setCycleCount(Animation.INDEFINITE);
        healTimer4.play();

        Tower tower6 = new Tower();
        tower6.setTeam(false);
        tower6.initGraphics(imgTower, "tower6", 0, 3500, 3300, 300.0, 300);
        tower6.resurrectWorld();
        towersB.add(tower6);
        buldings.add(tower6);
        Timeline healTimer6 = new Timeline(new KeyFrame(Duration.seconds(1), e -> tower6.healUnits()));
        healTimer6.setCycleCount(Animation.INDEFINITE);
        healTimer6.play();

        for (Unit unit : units) {
            if (unit == null) {
                continue;
            }
            if (unit.getTeam()) {
                unit.resurrect();
                unit.setPosition(basesA.get(0).x, basesA.get(0).y);
            } else {
                unit.resurrect();
                unit.setPosition(basesB.get(0).x, basesB.get(0).y);
            }
        }

        World world = new World(units);
        double initialW = scene != null && scene.getWidth() > 0 ? scene.getWidth() : VIEWPORT_WIDTH;
        double initialH = scene != null && scene.getHeight() > 0 ? scene.getHeight() : VIEWPORT_HEIGHT;
        miniMapOverlay = new MiniMapOverlay(WORLD_WIDTH, WORLD_HEIGHT, initialW, initialH, target -> {
            double currentViewportWidth = scene != null && scene.getWidth() > 0 ? scene.getWidth() : VIEWPORT_WIDTH;
            double currentViewportHeight = scene != null && scene.getHeight() > 0 ? scene.getHeight() : VIEWPORT_HEIGHT;
            updateCameraPosition(target[0] - currentViewportWidth / 2.0, target[1] - currentViewportHeight / 2.0);
        });
        miniMapOverlay.bindToScene(scene);
        overlayPane.getChildren().add(miniMapOverlay.getPane());

        scene.setOnMouseClicked(event -> {
            if (miniMapOverlay != null) {
                double miniMapX = miniMapOverlay.getPane().getLayoutX();
                double miniMapY = miniMapOverlay.getPane().getLayoutY();
                double miniMapWidth = miniMapOverlay.getPane().getPrefWidth();
                double miniMapHeight = miniMapOverlay.getPane().getPrefHeight();
                if (event.getX() >= miniMapX && event.getX() <= miniMapX + miniMapWidth && event.getY() >= miniMapY && event.getY() <= miniMapY + miniMapHeight) {
                    return;
                }
            }

            double worldMouseX = event.getX() + cameraX;
            double worldMouseY = event.getY() + cameraY;

            if (event.getButton() == MouseButton.PRIMARY) {
                for (int i = units.size() - 1; i >= 0; i--) {
                    Unit unit = units.get(i);
                    if (unit.tryActivate(worldMouseX, worldMouseY)) {
                        break;
                    }
                }
            } else if (event.getButton() == MouseButton.SECONDARY) {
                for (int i = units.size() - 1; i >= 0; i--) {
                    Unit unit = units.get(i);
                    if (unit.getImage().getBoundsInParent().contains(worldMouseX, worldMouseY)) {
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
                double camDx = 0, camDy = 0;
                double camSpeedMultiplier = 4.5;

                for (KeyCode code : new ArrayList<>(keysPressed.keySet())) {
                    if (keysPresses.containsKey(code)) {
                        double step = keysPresses.get(code);
                        if (code == KeyCode.W) {
                            dy += step;
                        } else if (code == KeyCode.S) {
                            dy += step;
                        } else if (code == KeyCode.A) {
                            dx += step;
                        } else if (code == KeyCode.D) {
                            dx += step;
                        } else if (code == KeyCode.UP) {
                            camDy += step * camSpeedMultiplier;
                        } else if (code == KeyCode.DOWN) {
                            camDy += step * camSpeedMultiplier;
                        } else if (code == KeyCode.LEFT) {
                            camDx += step * camSpeedMultiplier;
                        } else if (code == KeyCode.RIGHT) {
                            camDx += step * camSpeedMultiplier;
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
                                newUnit.spawnAtTeamBase();
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
                    } else if (code == KeyCode.F) {
                        if (unitSearchWindow.isVisible()) {
                            unitSearchWindow.setVisible(false);
                        } else {
                            unitSearchWindow.show();
                        }
                        keysPressed.remove(KeyCode.F);
                    } else if (code == KeyCode.V) {
                        // Toggle inverse mode for ALL active Warriors (per-instance flag)
                        for (Unit unit : units) {
                            if (unit.isActive() && unit.getClass() == Warrior.class) {
                                Warrior w = (Warrior) unit;
                                w.setInverseMode(!w.isInverseMode());
                            }
                        }
                        keysPressed.remove(KeyCode.V);
                    }
                }

                if (camDx != 0 || camDy != 0) {
                    updateCameraPosition(cameraX + camDx, cameraY + camDy);
                }

                for (World world : new ArrayList<>(buldings)) {
                    if (world.getHealth() <= 0) {
                        world.removeBuildingFromGame();
                    }
                }

                updateHud();
                world.worldLogic();
                miniMapOverlay.update(units, buldings);

                for (World building : new ArrayList<>(buldings)) {
                    building.intersect();
                }
                
                for (Unit unit : units) {
                    if (unit.isActive()) {
                        unit.move(dx, dy);
                    }
                    if (unit instanceof Warrior) {
                        Warrior w = (Warrior) unit;
                        if (w.isInverseMode()) {
                            w.logicInverse();
                        } else {
                            w.logic();
                        }
                    } else {
                        unit.logic();
                    }
                }

                unitInvetorWindow.updateFromUnits(units);
                
            }
        };
        gameLoop.start();

        stage.setScene(scene);
        stage.show();

        scene.widthProperty().addListener((obs, oldVal, newVal) -> {
            double w = newVal.doubleValue();
            cameraViewport.setPrefWidth(w);
            cameraViewport.setMinWidth(w);
            cameraViewport.setMaxWidth(w);
            if (cameraViewport.getClip() instanceof Rectangle) {
                ((Rectangle) cameraViewport.getClip()).setWidth(w);
            }
            if (miniMapOverlay != null) {
                miniMapOverlay.setViewportSize(w, scene.getHeight());
            }
            updateCameraPosition(cameraX, cameraY);
        });
        scene.heightProperty().addListener((obs, oldVal, newVal) -> {
            double h = newVal.doubleValue();
            cameraViewport.setPrefHeight(h);
            cameraViewport.setMinHeight(h);
            cameraViewport.setMaxHeight(h);
            if (cameraViewport.getClip() instanceof Rectangle) {
                ((Rectangle) cameraViewport.getClip()).setHeight(h);
            }
            if (miniMapOverlay != null) {
                miniMapOverlay.setViewportSize(scene.getWidth(), h);
            }
            updateCameraPosition(cameraX, cameraY);
        });
    }

    private void cloneActiveUnit() {
        Unit sourceUnit = null;
        for (int i = units.size() - 1; i >= 0; i--) {
            Unit candidate = units.get(i);
            if (candidate != null && candidate.isActive()) {
                sourceUnit = candidate;
                break;
            }
        }

        if (sourceUnit == null) {
            handledActionKeys.remove(KeyCode.V);
            return;
        }

        try {
            Unit clonedUnit = (Unit) sourceUnit.clone();
            units.add(clonedUnit);
            clonedUnit.setPosition(sourceUnit.x + 20, sourceUnit.y + 20);
            clonedUnit.resurrect();
            System.out.println("CLONE: Unit cloned successfully!");
        } catch (CloneNotSupportedException ex) {
            System.err.println("Clone not supported: " + ex.getMessage());
            ex.printStackTrace();
        }
    }

    private void updateHud() {
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

    private void updateCameraPosition(double newCameraX, double newCameraY) {
        double currentViewportWidth = scene != null && scene.getWidth() > 0 ? scene.getWidth() : VIEWPORT_WIDTH;
        double currentViewportHeight = scene != null && scene.getHeight() > 0 ? scene.getHeight() : VIEWPORT_HEIGHT;
        double maxCameraX = Math.max(0.0, WORLD_WIDTH - currentViewportWidth);
        double maxCameraY = Math.max(0.0, WORLD_HEIGHT - currentViewportHeight);
        cameraX = Math.max(0.0, Math.min(newCameraX, maxCameraX));
        cameraY = Math.max(0.0, Math.min(newCameraY, maxCameraY));
        group.setLayoutX(-cameraX);
        group.setLayoutY(-cameraY);
        if (miniMapOverlay != null) {
            miniMapOverlay.setCameraPosition(cameraX, cameraY);
        }
    }

}
