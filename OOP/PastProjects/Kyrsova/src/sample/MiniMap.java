package sample;

import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import objects.micro.Shopper;
import objects.secondMacro.Building;
import objects.thirdMacro.City;
import sample.windows.preferencesWindow.Preferences;

import java.util.HashMap;

public class MiniMap {
    final static private double SCALE = 0.1;
    private Pane pane;
    private HashMap<Shopper, ImageView> shoppersMap;
    private HashMap<Building, ImageView> buildingsMap;
    private Rectangle mainArea;

    public MiniMap() {
        this.pane = new Pane();
        this.pane.setMinWidth(City.ROOT_WIDTH * MiniMap.SCALE);
        this.pane.setMinHeight(City.ROOT_HEIGHT * MiniMap.SCALE);
        shoppersMap = new HashMap<>();
        buildingsMap = new HashMap<>();


        Rectangle rectangle = new Rectangle(0, 0, pane.getMinWidth(), pane.getMinHeight());
        rectangle.setFill(Color.LIGHTGREY);
        rectangle.setStroke(Color.BLACK);
        rectangle.setStrokeWidth(2);
        Label label = new Label("Map");
        label.setFont(new Font("Segoe UI Black Italic", 16));
        label.setLayoutX(pane.getMinWidth() / 2.1);

        mainArea = new Rectangle(0, 0, Main.SCENE_WIDTH * MiniMap.SCALE, Main.SCENE_HEIGHT * MiniMap.SCALE);
        mainArea.setFill(Color.TRANSPARENT);
        mainArea.setStrokeWidth(2);
        mainArea.setStroke(Color.YELLOW);
        this.pane.getChildren().addAll(rectangle, label, mainArea);

        this.pane.setOnMousePressed(event -> {
            this.moveTo(event.getX(), event.getY());
        });
    }

    public void moveTo(double x, double y) {
        if (x < mainArea.getWidth() / 2) {
            Main.getScrollPane().setHvalue(0);
        } else if (x > pane.getWidth() - mainArea.getWidth() / 2) {
            Main.getScrollPane().setHvalue(1);
        } else Main.getScrollPane().setHvalue(x / pane.getWidth());

        if (y < mainArea.getHeight() / 2) {
            Main.getScrollPane().setVvalue(0);
        } else if (y > pane.getHeight() - mainArea.getHeight() / 2) {
            Main.getScrollPane().setVvalue(1);
        } else Main.getScrollPane().setVvalue(y / pane.getHeight());
    }

    public Pane getPane() {
        return pane;
    }

    public Rectangle getMainArea() {
        return mainArea;
    }

    public static double getSCALE() {
        return SCALE;
    }

    public void addShopper(Shopper shopper) {
        ImageView imageView = new ImageView(shopper.getShopperImage().getImage());
        imageView.setLayoutX(shopper.getXChord() * MiniMap.SCALE);
        imageView.setLayoutY(shopper.getYChord() * MiniMap.SCALE);
        imageView.setPreserveRatio(true);
        imageView.setFitHeight(190 * MiniMap.SCALE);
        shoppersMap.put(shopper, imageView);
        pane.getChildren().add(imageView);
    }

    public void deleteAShopper(Shopper shopper) {
        pane.getChildren().remove(shoppersMap.get(shopper));
        shoppersMap.remove(shopper);
    }

    public void addBuilding(Building building) {
        ImageView imageView = new ImageView(building.getBuildingImage().getImage());
        imageView.setPreserveRatio(true);
        imageView.setFitHeight(building.getBuildingImage().getFitHeight() * MiniMap.SCALE);
        imageView.setLayoutX(building.getXChord() * MiniMap.SCALE);
        imageView.setLayoutY(building.getYChord() * MiniMap.SCALE);

        buildingsMap.put(building, imageView);
        pane.getChildren().add(imageView);
    }

    public void deleteABuilding(Building building) {
        pane.getChildren().remove(buildingsMap.get(building));
        buildingsMap.remove(building);
    }

    public void updateMap() {
        if (Preferences.isMAP()) {
            pane.setOpacity(1);
            mainArea.setWidth(Main.getScene().getWidth()* MiniMap.SCALE);
            mainArea.setHeight(Main.getScene().getHeight()*MiniMap.SCALE);
        } else pane.setOpacity(0);
        for (Shopper shopper : Main.getCity().getShoppers()) {
            ImageView imageView = shoppersMap.get(shopper);
            imageView.setLayoutX(shopper.getXChord() * MiniMap.SCALE);
            imageView.setLayoutY(shopper.getYChord() * MiniMap.SCALE);
        }
    }
}
