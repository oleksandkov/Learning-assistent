package sample;

import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.paint.ImagePattern;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;

import sample.micro.Scout;
import sample.secondMacro.Planet;

import java.util.HashMap;

public class MiniMap {
    final static private double SCALE = 0.1;
    private final Pane pane;
    private final HashMap<Scout, ImageView> shipsMap;
    private final HashMap<Planet, Group> planetMap;


    private final Rectangle mapArea;
    private boolean mapOpacity;


    private Image miniMapBackground;
    private Image miniMapBorder;

    private final Rectangle border;


    //getters
    public Pane getPane() {
        return pane;
    }
    public Rectangle getMapArea() {
        return mapArea;
    }
    public static double getSCALE() {
        return SCALE;
    }

    public MiniMap() {
        this.pane = new Pane();

        this.pane.setMinWidth(World.getRootWidth() * MiniMap.SCALE);
        this.pane.setMinHeight(World.getRootHeight() * MiniMap.SCALE);


        shipsMap = new HashMap<>();
        planetMap = new HashMap<>();

        try {
            miniMapBackground = new Image("images/background.png");
            miniMapBorder = new Image("images/miniMapBorder.png");
        } catch (Exception e) {
            System.out.println("Не удалось загрузить изображение!");
        }

        this.mapOpacity = false;
        Rectangle rectangle = new Rectangle(0, 0, pane.getMinWidth(), pane.getMinHeight());
        rectangle.setFill(new ImagePattern(miniMapBackground));
//        rectangle.setStroke(Color.BLACK);
//        rectangle.setStrokeWidth(5);

        border = new Rectangle(-50,-26,pane.getMinWidth()+110, pane.getMinHeight()+53);
        border.setFill(new ImagePattern(miniMapBorder));

        mapArea = new Rectangle(0, 0, Main.getSceneWidth() * MiniMap.SCALE, Main.getSceneHeight() * MiniMap.SCALE);
        mapArea.setFill(Color.TRANSPARENT);
        mapArea.setStrokeWidth(2);
        mapArea.setStroke(Color.YELLOW);

        this.pane.getChildren().addAll(rectangle,border, mapArea);

        pane.addEventHandler(MouseEvent.MOUSE_CLICKED, event -> {
            if (event.getButton() == MouseButton.PRIMARY) {
                if (!mapOpacity)
                    moveTo(event.getX(), event.getY());
            }
            if (event.getButton() == MouseButton.SECONDARY){
                mapOpacity=!mapOpacity;
                if (mapOpacity){
                    this.pane.setOpacity(0);
                }else {
                    this.pane.setOpacity(1);
                }
            }
        });
    }

    public void moveTo(double x, double y) {
        if (x < mapArea.getWidth() / 2) {
            Main.getScrollPane().setHvalue(0);
        } else if (x > pane.getWidth() - mapArea.getWidth() / 2) {
            Main.getScrollPane().setHvalue(1);
        } else Main.getScrollPane().setHvalue(x / pane.getWidth());

        if (y < mapArea.getHeight() / 2) {
            Main.getScrollPane().setVvalue(0);
        } else if (y > pane.getHeight() - mapArea.getHeight() / 2) {
            Main.getScrollPane().setVvalue(1);
        } else Main.getScrollPane().setVvalue(y / pane.getHeight());
    }



    public void addShip(Scout ship) {
        ImageView imageView;

        switch (ship.getType()) {
            case "Scout":
                if (ship.getSide().equals("Green")){
                    imageView = new ImageView(new Image("images/scout.png"));
                }else
                    imageView = new ImageView(new Image("images/scoutSideTwo.png"));
                break;
            case "Heavy":
                if (ship.getSide().equals("Green")){
                    imageView = new ImageView(new Image("images/heavy.png"));
                }else
                    imageView = new ImageView(new Image("images/heavySideTwo.png"));
                break;
            default:
                if (ship.getSide().equals("Green")){
                    imageView = new ImageView(new Image("images/starShip.png"));
                }else
                    imageView = new ImageView(new Image("images/starShipSideTwo.png"));
                break;
        }

        imageView.setLayoutX(ship.getChordX() * SCALE);
        imageView.setLayoutY(ship.getChordY() * SCALE);
        imageView.setPreserveRatio(true);
        imageView.setFitHeight(190 * SCALE);
        shipsMap.put(ship, imageView);
        pane.getChildren().add(imageView);
    }

    public void deleteShip(Scout ship){
        pane.getChildren().remove(shipsMap.get(ship));
    }

    public void addPlanet(Planet planet){
        ImageView imageView;
        Group group;
        Circle circle = new Circle(planet.getActiveZone().getRadius() * MiniMap.SCALE);
        switch (planet.getPlanetType()){
            case "GreenPlanet":
                imageView = new ImageView(new Image("images/greenPlanet.png"));
                imageView.setPreserveRatio(true);
                circle.setFill(Color.LIGHTGREEN);
                circle.setOpacity(0.4);
                imageView.setFitHeight(planet.getPlanetImage().getFitHeight() * MiniMap.SCALE);
                break;
            case "RedPlanet":
                imageView = new ImageView(new Image("images/redPlanet.png"));
                imageView.setPreserveRatio(true);
                circle.setFill(Color.LIGHTCORAL);
                circle.setOpacity(0.4);
                imageView.setFitHeight(planet.getPlanetImage().getFitHeight() * MiniMap.SCALE);
                break;
            default:
                imageView = new ImageView(new Image("images/moonSTO.png"));
                imageView.setPreserveRatio(true);
                circle.setFill(Color.LIGHTCYAN);
                circle.setOpacity(0.4);
                imageView.setFitHeight(planet.getPlanetImage().getFitHeight() * MiniMap.SCALE);
                break;
        }
        imageView.setLayoutX(planet.getChordX() * MiniMap.SCALE);
        imageView.setLayoutY(planet.getChordY() * MiniMap.SCALE);
        circle.setLayoutX((planet.getChordX() + 170) * MiniMap.SCALE);
        circle.setLayoutY((planet.getChordY() + 170) * MiniMap.SCALE);
        group = new Group(imageView, circle);

        planetMap.put(planet, group);
        pane.getChildren().addAll(imageView, circle);
    }

    public void deletePlanet(Planet planet){
        pane.getChildren().remove(planetMap.get(planet));
        planetMap.remove(planet);
    }

    public void updateMap() {
        for (Scout ship : Main.getWorld().getShips()) {
            ImageView imageView = shipsMap.get(ship);
            imageView.setLayoutX(ship.getChordX() * MiniMap.SCALE);
            imageView.setLayoutY(ship.getChordY() * MiniMap.SCALE);
        }
        this.pane.getChildren().remove(border);
        this.pane.getChildren().add(border);
    }


}



