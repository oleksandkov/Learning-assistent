package org.example.laba5;

import java.util.ArrayList;
import java.util.Collections;
import java.util.IdentityHashMap;
import java.util.Iterator;
import java.util.Set;
import java.util.function.Consumer;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Polygon;
import javafx.scene.shape.Rectangle;
import org.example.laba5.Unit.Unit;
import org.example.laba5.Unit.Centurio;
import org.example.laba5.Unit.Pretorio;

public class MiniMapOverlay {
    private static final double SCALE = 0.08;
    private static final double MARGIN = 12.0;

    private final Pane root;
    private final Pane buildingLayer;
    private final Pane unitLayer;
    private final Rectangle background;
    private final Rectangle border;
    private final Rectangle viewportRect;
    private final Consumer<double[]> onNavigate;

    private final IdentityHashMap<Unit, Polygon> unitMarkers = new IdentityHashMap<>();
    private final IdentityHashMap<World, Polygon> buildingMarkers = new IdentityHashMap<>();

    private double worldWidth;
    private double worldHeight;
    private double cameraX;
    private double cameraY;
    private double viewportWidth;
    private double viewportHeight;

    public MiniMapOverlay(double initialWorldWidth, double initialWorldHeight, double viewportWidth, double viewportHeight, Consumer<double[]> onNavigate) {
        this.worldWidth = Math.max(1.0, initialWorldWidth);
        this.worldHeight = Math.max(1.0, initialWorldHeight);
        this.viewportWidth = Math.max(1.0, viewportWidth);
        this.viewportHeight = Math.max(1.0, viewportHeight);
        this.onNavigate = onNavigate;

        double mapWidth = this.worldWidth * SCALE;
        double mapHeight = this.worldHeight * SCALE;

        root = new Pane();
        root.setPrefSize(mapWidth, mapHeight + 20);
        root.setMinSize(mapWidth, mapHeight + 20);
        root.setMaxSize(mapWidth, mapHeight + 20);
        root.setBackground(new Background(new BackgroundFill(Color.TRANSPARENT, CornerRadii.EMPTY, Insets.EMPTY)));

        background = new Rectangle(0, 20, mapWidth, mapHeight);
        background.setFill(Color.rgb(14, 22, 30, 0.85));
        background.setArcWidth(12);
        background.setArcHeight(12);

        border = new Rectangle(0, 20, mapWidth, mapHeight);
        border.setFill(Color.TRANSPARENT);
        border.setStroke(Color.rgb(120, 200, 255, 0.9));
        border.setStrokeWidth(2.0);
        border.setArcWidth(12);
        border.setArcHeight(12);

        viewportRect = new Rectangle();
        viewportRect.setFill(Color.color(0.2, 0.9, 1.0, 0.14));
        viewportRect.setStroke(Color.web("#7ef0ff"));
        viewportRect.setStrokeWidth(1.6);
        viewportRect.setArcWidth(8);
        viewportRect.setArcHeight(8);

        buildingLayer = new Pane();
        buildingLayer.setLayoutY(20);
        buildingLayer.setPickOnBounds(false);

        unitLayer = new Pane();
        unitLayer.setLayoutY(20);
        unitLayer.setPickOnBounds(false);

        root.getChildren().addAll(background, buildingLayer, unitLayer, viewportRect, border);

        root.setOnMouseClicked(event -> {
            double mx = clamp(event.getX(), 0, background.getWidth());
            double my = clamp(event.getY() - 20, 0, background.getHeight());
            double targetX = miniToWorldX(mx);
            double targetY = miniToWorldY(my);
            if (onNavigate != null) {
                onNavigate.accept(new double[] { targetX, targetY });
            }
        });
    }

    public Pane getPane() {
        return root;
    }

    public void bindToScene(Scene scene) {
        if (scene == null) {
            return;
        }
        positionInCorner(scene);
        scene.widthProperty().addListener((obs, oldVal, newVal) -> positionInCorner(scene));
        scene.heightProperty().addListener((obs, oldVal, newVal) -> positionInCorner(scene));
    }

    public void toggleVisible() {
        root.setVisible(!root.isVisible());
    }

    public void update(ArrayList<Unit> units, ArrayList<World> buildings) {
        ArrayList<Unit> unitsSnapshot = units == null ? new ArrayList<>() : new ArrayList<>(units);
        ArrayList<World> buildingsSnapshot = buildings == null ? new ArrayList<>() : new ArrayList<>(buildings);

        recomputeWorldBounds(unitsSnapshot, buildingsSnapshot);
        updateBuildings(buildingsSnapshot);
        updateUnits(unitsSnapshot);
        updateViewportRect();
    }

    public void setCameraPosition(double cameraX, double cameraY) {
        this.cameraX = cameraX;
        this.cameraY = cameraY;
        updateViewportRect();
    }

    public void setViewportSize(double width, double height) {
        this.viewportWidth = Math.max(1.0, width);
        this.viewportHeight = Math.max(1.0, height);
        updateViewportRect();
    }

    public void navigateToWorldPoint(double targetX, double targetY) {
        double desiredX = targetX - viewportWidth / 2.0;
        double desiredY = targetY - viewportHeight / 2.0;
        double maxX = Math.max(0.0, worldWidth - viewportWidth);
        double maxY = Math.max(0.0, worldHeight - viewportHeight);
        setCameraPosition(clamp(desiredX, 0.0, maxX), clamp(desiredY, 0.0, maxY));
    }

    private void positionInCorner(Scene scene) {
        root.setLayoutX(scene.getWidth() - root.getPrefWidth() - MARGIN);
        root.setLayoutY(MARGIN);
    }

    private void recomputeWorldBounds(ArrayList<Unit> units, ArrayList<World> buildings) {
        double maxX = worldWidth;
        double maxY = worldHeight;

        for (World world : buildings) {
            if (world == null) {
                continue;
            }
            double w = world.image == null ? 200.0 : world.image.getWidth();
            double h = world.image == null ? 200.0 : world.image.getHeight();
            maxX = Math.max(maxX, world.x + w + 60.0);
            maxY = Math.max(maxY, world.y + h + 60.0);
        }

        for (Unit unit : units) {
            if (unit == null) {
                continue;
            }
            double uw = unit.getImage() == null ? 100.0 : unit.getImage().getFitWidth();
            double uh = unit.getImage() == null ? 100.0 : unit.getImage().getFitHeight();
            maxX = Math.max(maxX, unit.x + uw + 40.0);
            maxY = Math.max(maxY, unit.y + uh + 40.0);
        }

        worldWidth = Math.max(1.0, maxX);
        worldHeight = Math.max(1.0, maxY);
    }

    private void updateBuildings(ArrayList<World> buildings) {
        Set<World> alive = Collections.newSetFromMap(new IdentityHashMap<>());

        for (World world : buildings) {
            if (world == null) {
                continue;
            }
            alive.add(world);

            Polygon marker = buildingMarkers.get(world);
            if (marker == null) {
                marker = new Polygon();
                marker.setOpacity(0.8);
                buildingMarkers.put(world, marker);
                buildingLayer.getChildren().add(marker);
            }

            double size = getBuildingMarkerSize(world);
            updateBuildingMarkerShape(marker, world, size);
            marker.setFill(world.getTeam() ? Color.web("#6fe3a4") : Color.web("#d46a6a"));
            marker.setStroke(Color.color(0, 0, 0, 0.45));
            marker.setStrokeWidth(1.0);
            marker.setLayoutX(worldToMiniX(world.x) + size * 0.5);
            marker.setLayoutY(worldToMiniY(world.y) + size * 0.5);
        }

        Iterator<World> it = buildingMarkers.keySet().iterator();
        while (it.hasNext()) {
            World world = it.next();
            if (!alive.contains(world)) {
                Polygon marker = buildingMarkers.get(world);
                buildingLayer.getChildren().remove(marker);
                it.remove();
            }
        }
    }

    private void updateUnits(ArrayList<Unit> units) {
        Set<Unit> alive = Collections.newSetFromMap(new IdentityHashMap<>());

        for (Unit unit : units) {
            if (unit == null || Boolean.TRUE.equals(unit.getDead()) || unit.getImage() == null) {
                continue;
            }
            alive.add(unit);

            Polygon marker = unitMarkers.get(unit);
            if (marker == null) {
                marker = new Polygon();
                unitMarkers.put(unit, marker);
                unitLayer.getChildren().add(marker);
            }

            double size = getUnitMarkerRadius(unit) * 2.0;
            updateUnitMarkerShape(marker, size);
            marker.setLayoutX(worldToMiniX(unit.x + 10));
            marker.setLayoutY(worldToMiniY(unit.y + 10));
            marker.setFill(unit.getTeam() ? Color.web("#5fe2b0") : Color.web("#ff7a7a"));
            marker.setStroke(unit.isActive() ? Color.web("#ffd166") : Color.TRANSPARENT);
            marker.setStrokeWidth(unit.isActive() ? 1.2 : 0.0);
            marker.setOpacity(0.92);
        }

        Iterator<Unit> it = unitMarkers.keySet().iterator();
        while (it.hasNext()) {
            Unit unit = it.next();
            if (!alive.contains(unit)) {
                Polygon marker = unitMarkers.get(unit);
                unitLayer.getChildren().remove(marker);
                it.remove();
            }
        }
    }

    private double worldToMiniX(double worldX) {
        return clamp((worldX / worldWidth) * background.getWidth(), 0, background.getWidth());
    }

    private double worldToMiniY(double worldY) {
        return clamp((worldY / worldHeight) * background.getHeight(), 0, background.getHeight());
    }

    private double miniToWorldX(double miniX) {
        return (miniX / background.getWidth()) * worldWidth;
    }

    private double miniToWorldY(double miniY) {
        return (miniY / background.getHeight()) * worldHeight;
    }

    private double getUnitMarkerRadius(Unit unit) {
        if (unit instanceof Pretorio) {
            return 3.5;
        }
        if (unit instanceof Centurio) {
            return 3.0;
        }
        return 2.6;
    }

    private double getBuildingMarkerSize(World world) {
        if (world instanceof Base) {
            return 8.0;
        }
        if (world instanceof Tower) {
            return 6.0;
        }
        return 5.6;
    }

    private void updateUnitMarkerShape(Polygon marker, double size) {
        double half = size * 0.5;
        marker.getPoints().setAll(
            0.0, -half,
            half, 0.0,
            0.0, half,
            -half, 0.0
        );
    }

    private void updateBuildingMarkerShape(Polygon marker, World world, double size) {
        double half = size * 0.5;
        if (world instanceof Base) {
            marker.getPoints().setAll(
                -half, -half,
                half, -half,
                half, half,
                -half, half
            );
        } else if (world instanceof Tower) {
            marker.getPoints().setAll(
                0.0, -half,
                half, half,
                -half, half
            );
        } else {
            marker.getPoints().setAll(
                0.0, -half,
                half, 0.0,
                0.0, half,
                -half, 0.0
            );
        }
    }

    private void updateViewportRect() {
        viewportRect.setVisible(true);
        double viewportMiniWidth = viewportWidth / worldWidth * background.getWidth();
        double viewportMiniHeight = viewportHeight / worldHeight * background.getHeight();
        viewportRect.setX(worldToMiniX(cameraX));
        viewportRect.setY(20 + worldToMiniY(cameraY));
        viewportRect.setWidth(viewportMiniWidth);
        viewportRect.setHeight(viewportMiniHeight);
    }

    private double clamp(double value, double min, double max) {
        return Math.max(min, Math.min(max, value));
    }
}
