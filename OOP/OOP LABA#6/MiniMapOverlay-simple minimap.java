package org.example.laba5;

import java.util.ArrayList;
import java.util.function.Consumer;

import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.SnapshotParameters;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.image.WritableImage;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;

public class MiniMapOverlay {
    private static final double SCALE = 0.08;
    private static final double MARGIN = 12.0;

    private final Pane root;
    private final Rectangle background;
    private final Rectangle border;
    private final Rectangle viewportRect;
    private final Label title;
    private final Consumer<double[]> onNavigate;

    private ImageView imgviewmap;

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
        background.setFill(Color.rgb(23, 31, 40, 0.75));
        background.setArcWidth(10);
        background.setArcHeight(10);

        border = new Rectangle(0, 20, mapWidth, mapHeight);
        border.setFill(Color.TRANSPARENT);
        border.setStroke(Color.rgb(186, 220, 255, 0.85));
        border.setStrokeWidth(1.5);
        border.setArcWidth(10);
        border.setArcHeight(10);

        viewportRect = new Rectangle();
        viewportRect.setFill(Color.color(1.0, 1.0, 0.0, 0.12));
        viewportRect.setStroke(Color.GOLD);
        viewportRect.setStrokeWidth(1.8);
        viewportRect.setArcWidth(6);
        viewportRect.setArcHeight(6);

        title = new Label("MiniMap (M to toggle)");
        title.setTextFill(Color.ALICEBLUE);
        title.setLayoutX(4);
        title.setLayoutY(0);

        imgviewmap = new ImageView();
        imgviewmap.setFitWidth(mapWidth);
        imgviewmap.setFitHeight(mapHeight);
        imgviewmap.setLayoutX(0);
        imgviewmap.setLayoutY(20);

        // Clip to keep the snapshot inside the rounded corners
        Rectangle clip = new Rectangle(mapWidth, mapHeight);
        clip.setArcWidth(10);
        clip.setArcHeight(10);
        imgviewmap.setClip(clip);

        root.getChildren().addAll(background, imgviewmap, viewportRect, border, title);

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
        if (root.isVisible() && HelloApplication.group != null) {
            final WritableImage snapshot = HelloApplication.group.snapshot(new SnapshotParameters(), null);
            imgviewmap.setImage(snapshot);
        }
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
