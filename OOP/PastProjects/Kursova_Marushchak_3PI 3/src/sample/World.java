package sample;

import javafx.scene.image.Image;
import javafx.scene.layout.Pane;

import javafx.scene.paint.Color;
import javafx.scene.paint.ImagePattern;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import sample.micro.Scout;
import sample.secondMacro.Planet;


import java.util.ArrayList;

public class World {
    private final static int rootHeight = 3000;
    private final static int rootWidth = 5000;

    private static final Pane root = new Pane();

    private final ArrayList<Scout> ships;
    private final ArrayList<Planet> planets;

    private final Text redCounterText;
    private final Text blueCounterText;

    private final  Text moveToCoffeeText;
    private final  Text stayHomeText;
    private final  Text removeFromWorldText;
    private final  Text closePlanetsText;


    private final MiniMap miniMap;




    //getters
    public static Pane getRoot() {
        return root;
    }
    public ArrayList<Scout> getShips() {
        return ships;
    }
    public ArrayList<Planet> getPlanets() {
        return planets;
    }
    public static int getRootHeight() {
        return rootHeight;
    }
    public static int getRootWidth() {
        return rootWidth;
    }
    public MiniMap getMiniMap() {
        return miniMap;
    }
    public Text getRedCounterText() {
        return redCounterText;
    }
    public Text getBlueCounterText() {
        return blueCounterText;
    }
    public Text getClosePlanetsText() {
        return closePlanetsText;
    }
    public Text getMoveToCoffeeText() {
        return moveToCoffeeText;
    }
    public Text getRemoveFromWorldText() {
        return removeFromWorldText;
    }
    public Text getStayHomeText() {
        return stayHomeText;
    }

    public World(){
        root.setMinWidth(rootWidth);
        root.setMinHeight(rootHeight);

        Rectangle rectangle = new Rectangle(rootWidth, rootHeight);
        Image img = new Image("images/background.png");
        rectangle.setFill(new ImagePattern(img));
        root.getChildren().add(rectangle);

        //redCounter
        redCounterText = new Text();
        redCounterText.setFill(Color.WHITE);
        redCounterText.setFont(new Font("Monaco", 24));
        root.getChildren().add(redCounterText);

        //blueCounter
        blueCounterText = new Text();
        blueCounterText.setFill(Color.WHITE);
        blueCounterText.setFont(new Font("Monaco", 24));
        root.getChildren().add(blueCounterText);

        //moveToCoffeeText
        moveToCoffeeText = new Text();
        moveToCoffeeText.setFill(Color.WHITE);
        moveToCoffeeText.setFont(new Font("Monaco", 24));
        root.getChildren().add(moveToCoffeeText);

        //stayHomeText
        stayHomeText = new Text();
        stayHomeText.setFill(Color.WHITE);
        stayHomeText.setFont(new Font("Monaco", 24));
        root.getChildren().add(stayHomeText);

        //removeFromWorldText
        removeFromWorldText = new Text();
        removeFromWorldText.setFill(Color.WHITE);
        removeFromWorldText.setFont(new Font("Monaco", 24));
        root.getChildren().add(removeFromWorldText);

        //closePlanetsText
        closePlanetsText = new Text();
        closePlanetsText.setFill(Color.WHITE);
        closePlanetsText.setFont(new Font("Monaco", 24));
        root.getChildren().add(closePlanetsText);


        this.ships = new ArrayList<>();
        this.planets = new ArrayList<>();

        this.miniMap = new MiniMap();
        root.getChildren().addAll(miniMap.getPane());
    }



    public void addNewShip(Scout ship, boolean rand){
        this.ships.add(ship);
        if (rand){
            ship.setChordX(Main.random.nextInt(World.rootWidth));
            ship.setChordY(Main.random.nextInt (World.rootHeight));
        }
        ship.setShipChord();
        root.getChildren().add(ship.getShipGroup());
        miniMap.addShip(ship);
    }

    public void deleteShip(Scout ship){
        this.miniMap.deleteShip(ship);
        root.getChildren().remove(ship.getShipGroup());
        this.ships.remove(ship);
        Scout.setNumberOfShips(Scout.getNumberOfShips() - 1);
    }

    public void addNewPlanet(Planet planet){
        this.planets.add(planet);
        planet.setPlanetChord();
        root.getChildren().add(planet.getPlanetGroup());
        miniMap.addPlanet(planet);

    }

    public void deletePlanet(Planet planet){
        this.miniMap.deletePlanet(planet);
        root.getChildren().remove(planet.getPlanetGroup());
        this.planets.remove(planet);
        Planet.setNumberOfPlanets(Planet.getNumberOfPlanets());
    }


    public void  miniMapInFront(){
        root.getChildren().remove(miniMap.getPane());
        root.getChildren().add(miniMap.getPane());
    }
}
