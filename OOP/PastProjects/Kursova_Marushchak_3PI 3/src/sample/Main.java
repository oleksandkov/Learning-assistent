package sample;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.ScrollPane;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;
import sample.micro.Heavy;
import sample.micro.Scout;
import sample.micro.StarShip;
import sample.secondMacro.GreenPlanet;
import sample.secondMacro.MoonSTO;
import sample.secondMacro.Planet;
import sample.secondMacro.RedPlanet;

import java.security.Key;
import java.util.Arrays;
import java.util.Date;
import java.util.Random;


public class Main extends Application{


    private static final int sceneWidth = 1920;
    private static final int sceneHeight = 1080;

    private static final World world = new World();
    private static final ScrollPane scrollPane = new ScrollPane(World.getRoot());
    private static final Scene scene = new Scene(scrollPane,  sceneWidth, sceneHeight);

    private static double scrollX;
    private static double scrollY;

    public static Random random = new Random(new Date().getTime());
    private static int deltaSpeed = 0;

    private static int counterOperateGreenPlanet;
    private static int counterOperateRedPlanet;
    private static int counterOperateMoonSTO;

    private static boolean moveToCoffee = false;
    private static boolean stayHome = false;
    private static boolean removeFromWorld = false;
    private static boolean closePlanets = false;
    private static boolean infoOpacity = false;


    public static int[] ArrayHealth;
    public static int[] ArrayCopy;
    private static Stage pStage;
    private static Scout scoutConfig;

    public static  boolean isCheack = false;
    public static Scout buffScout;
    public static double nextX;
    public static double nextY;

    public static int scoutX = 140;
    public static int scoutY = 2800;
    public static double heavyX = 10;
    public static double heavyY = 2800;
    public static double starShipX = 10;
    public static double starShipY = 100;

    public static int scoutCounter=0;
    public static int heavyCounter=0;
    public static int starShipCounter=0;

    public static boolean buttonZ = false;

    private static final String[] namesArray = {"Yoda", "Obi-Van", "Aalya Secura", "Kit Fisto","Plo Koon","Ahsoka Tano","Mace Windu","Qui-Gon Jinn","Anakin Skywalker",
            "Luke Skywalker","Coleman Trebor","Agen Kolar","Saesee Tiin","Yaddle","Barriss Offee","Shaak Ti","Jocasta Nu",
            "Sifo-Dyas","Rey","Ki-Adi-Mundi","Eekar Oki","E1-6RA","Silva","Thongla Jur","Tarre Vizsla","Lula","Holenesh"};


    //getters
    public static boolean getMoveToCoffe(){
        return moveToCoffee;
    }
    public static String[] getNamesArray() {
        return namesArray;
    }
    public static int getSceneHeight() {
        return sceneHeight;
    }
    public static int getSceneWidth() {
        return sceneWidth;
    }
    public static ScrollPane getScrollPane() {
        return scrollPane;
    }
    public static World getWorld() {
        return world;
    }
    public static int getDeltaSpeed() {return deltaSpeed;}
    public static Scene getScene() {
        return scene;
    }
    public static Scout getScoutConfig() {
        return scoutConfig;
    }

    //setters
    private void setPrimaryStage(Stage pStage){
        Main.pStage = pStage;
    }

    @Override
    public void start(Stage primaryStage) throws Exception{
        setPrimaryStage(primaryStage);
        scrollPane.setFitToHeight(true);
        scrollPane.setFitToWidth(true);

        world.addNewPlanet(new GreenPlanet());
        world.addNewPlanet(new RedPlanet());
        world.addNewPlanet(new MoonSTO());



        world.addNewShip(new Scout(), true);
        world.addNewShip(new Scout(), true);
        world.addNewShip(new Heavy(), true);
        world.addNewShip(new StarShip(), true);

        world.miniMapInFront();


        Parent parent = FXMLLoader.load(getClass().getResource("sample.fxml"));
        World.getRoot().getChildren().add(parent);

        scene.addEventHandler(KeyEvent.KEY_PRESSED, event ->{
            for (Scout ship : world.getShips()){
                if (ship.getIsActive()){
                    if (event.getCode() == KeyCode.W){
                        if (ship.getShipImage().getY() >=0)
                        ship.up(deltaSpeed);
                    }
                    if (event.getCode() == KeyCode.S){
                        if (ship.getShipImage().getY() + ship.getShipImage().getFitHeight()
                                <= World.getRoot().getHeight())
                        ship.down(deltaSpeed);
                    }
                    if (event.getCode() == KeyCode.A){
                        if (ship.getShipImage().getX() >=0)
                        ship.left(deltaSpeed);
                    }
                    if (event.getCode() == KeyCode.D){
                        if (ship.getShipImage().getX() + ship.getShipImage().getFitWidth() + 100
                                <= World.getRoot().getWidth())
                        ship.right(deltaSpeed);
                    }
                }
            }
            if (event.isShiftDown()){
                ++deltaSpeed;
            }
            if (event.isControlDown()){
                if (deltaSpeed >0)
                --deltaSpeed;
            }
            if (event.getCode() == KeyCode.DIGIT1){
                world.addNewShip(new Scout(), true);
                world.miniMapInFront();
            }
            if (event.getCode() == KeyCode.DIGIT2){
                world.addNewShip(new Heavy(), true);
                world.miniMapInFront();
            }
            if (event.getCode() == KeyCode.DIGIT3){
                world.addNewShip(new StarShip(), true);
                world.miniMapInFront();
            }
            if (event.getCode() == KeyCode.DIGIT4){
                world.addNewShip(new Scout(namesArray[Main.random.nextInt(namesArray.length)], false,5,500,5,200,"Red"), true);

                world.miniMapInFront();
            }
            if (event.getCode() == KeyCode.DIGIT5){
                world.addNewShip(new Heavy(namesArray[Main.random.nextInt(namesArray.length)], false,8,1000,8,200,"Red"), true);
                world.miniMapInFront();
            }
            if (event.getCode() == KeyCode.DIGIT6){
                world.addNewShip(new StarShip(namesArray[Main.random.nextInt(namesArray.length)], false,10,2000,11,200,"Red"), true);
                world.miniMapInFront();
            }

            if (event.getCode() == KeyCode.DELETE){
                for (int i = 0; i < world.getShips().size(); ++i){
                    Scout ship = world.getShips().get(i);
                    if (ship.getIsActive()){
                        world.deleteShip(ship);
                    }
                }
            }
            if (event.getCode() == KeyCode.ESCAPE){
                for (Scout ship : world.getShips()){
                    if (ship.getIsActive()){
                        ship.setActive();
                    }
                }
            }
            if (event.getCode() == KeyCode.ENTER){
                for (Scout ship : world.getShips()){
                    if (!ship.getIsActive()){
                        ship.setActive();
                    }
                }
            }
            if (event.getCode() == KeyCode.L){
                moveToCoffee=!moveToCoffee;
                for (Scout s : world.getShips()){
                    s.ToCoffee();
                }
            }
            if (event.getCode() == KeyCode.H){
                stayHome = !stayHome;
            }
            if (event.getCode() == KeyCode.J){
                removeFromWorld = !removeFromWorld;
            }
            if (event.getCode() == KeyCode.I ||
                    event.getCode() == KeyCode.INSERT ){
                try {
                    newWindow(primaryStage);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            if (event.getCode() == KeyCode.K){
                closePlanets = !closePlanets;
            }
            if (event.getCode() == KeyCode.DIGIT0){
                ArrayHealth = new int[world.getShips().size()];
                for (int i = 0; i<world.getShips().size(); ++i){
                    ArrayHealth[i] = world.getShips().get(i).getHealth();
                }

                System.out.println("*********************************************");
                System.out.println("Масив до використання java.utils.Arrays;");
                System.out.println("*********************************************");
                for (int i = 0; i<ArrayHealth.length; ++i){
                    System.out.print( "Type: " + world.getShips().get(i).getType() + "   Name: "
                            + world.getShips().get(i).getName().getText() + "   Health: ");
                    System.out.println(ArrayHealth[i]);
                }
                Arrays.sort(ArrayHealth);
                System.out.println("*********************************************");
                System.out.println("Масив після використання java.utils.Arrays;");
                System.out.println("*********************************************");
                for (int i = 0; i<ArrayHealth.length; ++i){
                    System.out.print( "Type: " + world.getShips().get(i).getType() +"   Name: "
                            + world.getShips().get(i).getName().getText() + "   Health: ");
                    System.out.println(ArrayHealth[i]);
                }
                System.out.println("*********************************************");
                System.out.println("Три персонажі з найнижчим рівнем життя");
                System.out.println("*********************************************");
                ArrayCopy = Arrays.copyOf(ArrayHealth, 3);
                for (int i = 0; i<ArrayCopy.length; i++){
                    System.out.print( "Type: " + world.getShips().get(i).getType() + "   Name: "
                            + world.getShips().get(i).getName().getText() + "   Health: ");
                    System.out.println(ArrayCopy[i]);
                }
            }
            if (event.getCode() == KeyCode.P){
                try {
                    charConfig(primaryStage);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            if (event.getCode() == KeyCode.TAB){
                infoOpacity = !infoOpacity;
                if (infoOpacity){
                    world.getRedCounterText().setOpacity(1);
                    world.getBlueCounterText().setOpacity(1);
                    world.getClosePlanetsText().setOpacity(1);
                    world.getRemoveFromWorldText().setOpacity(1);
                    world.getMoveToCoffeeText().setOpacity(1);
                    world.getStayHomeText().setOpacity(1);
                }else{
                    world.getRedCounterText().setOpacity(0);
                    world.getBlueCounterText().setOpacity(0);
                    world.getClosePlanetsText().setOpacity(0);
                    world.getRemoveFromWorldText().setOpacity(0);
                    world.getMoveToCoffeeText().setOpacity(0);
                    world.getStayHomeText().setOpacity(0);
                }
            }

            if (event.getCode() == KeyCode.DIGIT8){
                int counter2 = 0;
                for (Scout s : world.getShips()){
                    if (s.getIsActive()){
                        counter2++;
                    }
                }
                if (counter2==1){
                    for (Scout sc : world.getShips()){
                        if (sc.getIsActive()){
                            sc.sayHello();
                        }
                    }
                }
            }
            if (event.getCode() == KeyCode.Z){
                if (!buttonZ){
                    try {
                        setGeneralWindow(primaryStage);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    buttonZ=!buttonZ;
                }else{
                    for (Scout s: world.getShips()){
                        if (s.isGeneral()){
                            s.setGeneral(false);
                        }
                    }
                    try {
                        sWindow(primaryStage);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    buttonZ=!buttonZ;
                }

            }
        } );

        scene.addEventHandler(MouseEvent.MOUSE_CLICKED, mouseEvent -> {
            if (mouseEvent.getButton() == MouseButton.SECONDARY){
                int counter = 0;
                for (Scout s : world.getShips()){
                    if (s.getIsActive()){
                        counter++;
                    }
                }
                if (counter == 1){
                    for (Scout sc : world.getShips()){
                        if (sc.getIsActive()){
                            scoutConfig = sc;
                            try {
                                charConfig(primaryStage);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
            }
        });
        
        scrollPane.viewportBoundsProperty().addListener((observable, oldBounds, bounds) -> {

            Main.scrollX = -1 * (int) bounds.getMinX();
            Main.scrollY = -1 * (int) bounds.getMinY();

            parent.setLayoutX(scrollX);
            parent.setLayoutY(scrollY);

            updateChordINFO();


            if (world.getShips().size() == 0){
                currentStatusINFO();
            }

            world.getMiniMap().getPane().setLayoutX(scrollX + 1310);
            world.getMiniMap().getPane().setLayoutY(scrollY + scene.getHeight() - world.getMiniMap().getPane().getHeight() - 650);
            world.getMiniMap().getMapArea().setLayoutX(scrollX*MiniMap.getSCALE());
            world.getMiniMap().getMapArea().setLayoutY(scrollY*MiniMap.getSCALE());
        });


        primaryStage.setMaximized(true);
        primaryStage.setScene(scene);
        primaryStage.setTitle("**Space shooter**");



        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long now) {

                counterOperateGreenPlanet = 0;
                counterOperateRedPlanet = 0;
                counterOperateMoonSTO = 0;
                for (Scout ship : world.getShips()){
                    for (Planet p : world.getPlanets()){
                        switch (p.getPlanetType()){
                            case "GreenPlanet":
                                p.setCounterGreenText(counterOperateGreenPlanet);
                                break;
                            case "RedPlanet":
                                p.setCounterRedText(counterOperateRedPlanet);
                                break;
                            default:
                                p.setCounterMoonText(counterOperateMoonSTO);
                                break;
                        }
                        if(ship.getShipImage().getBoundsInParent().intersects(p.getPlanetImage().getBoundsInParent())){
                            switch (p.getPlanetType()){
                                case "GreenPlanet":
                                    if (ship.getSide().equals("Green")){
                                        ++counterOperateGreenPlanet;
                                        ship.healthUpGreen();
                                        p.setCounterGreenText(counterOperateGreenPlanet);

                                    }
                                    break;
                                case "RedPlanet":
                                    if (ship.getSide().equals("Red")){
                                        ++counterOperateRedPlanet;
                                        ship.healthUpRed();
                                        p.setCounterRedText(counterOperateRedPlanet);
                                    }
                                    break;
                                default:
                                    ++counterOperateMoonSTO;
                                    ship.healthUpMoon();
                                    p.setCounterMoonText(counterOperateMoonSTO);
                                    break;
                            }
                        }
                    }
                    for (Scout shipBound : world.getShips()){
                        if (ship != shipBound && ship.getShipImage().getBoundsInParent().intersects(shipBound.getShipImage().getBoundsInParent())){
                            Scout.shipInteraction(ship, shipBound);
                            if (ship.getHealth() * ship.getDeltaHealth() < 4 && removeFromWorld){
                                world.deleteShip(ship);
                            }
                        }
                    }

                    if (ship.isGeneral()){
                        ship.moveToSoldier();
                    }
                    if (ship.isCatch()){
                        switch (ship.getType()){
                            case "Scout":
                                scoutX+=140;
                                ship.moveDown(scoutX,scoutY,ship);

                                break;
                            case "Heavy":
                                heavyY-=140;
                                ship.moveLeft(heavyX,heavyY,ship);

                                break;
                            default:
                                starShipY+=140;
                                ship.moveUP(starShipX, starShipY, ship);
                                break;
                        }
                    }else{
                        if (!ship.isGeneral()){
                            if (closePlanets){
                                ship.autoMove();
                            }else{
                                if ((ship.getHealth()*ship.getDeltaHealth()) < 60 || moveToCoffee ||stayHome){
                                    ship.moveToPlanet();
                                }else{
                                    ship.autoMove();
                                }
                            }
                        }
                    }


                    ship.setShipChord();
                    currentStatusINFO();
                }
                world.getMiniMap().updateMap();
            }
        };

        timer.start();
        primaryStage.show();
    }


    public void newWindow(Stage primaryStage) throws Exception{
        Pane root1 = FXMLLoader.load(getClass().getResource("characterConfig.fxml"));
        Scene scene1 = new Scene(root1);
        Stage window = new Stage();
        window.setTitle("**Character editor**");
        window.setScene(scene1);
        window.show();
    }

    public void charConfig(Stage primaryStage) throws Exception{
        Pane root2 = FXMLLoader.load(getClass().getResource("charConf.fxml"));
        Scene scene1 = new Scene(root2);
        Stage window1 = new Stage();
        window1.setTitle("Current Configuration");
        window1.setScene(scene1);
        window1.show();
    }

    public void setGeneralWindow(Stage primaryStage) throws Exception{
        Pane root3 = FXMLLoader.load(getClass().getResource("setGeneral.fxml"));
        Scene scene2 = new Scene(root3);
        Stage window2 = new Stage();
        window2.setTitle("Current Configuration");
        window2.setScene(scene2);
        window2.show();
    }

    public void sWindow(Stage primaryStage) throws Exception{
        Pane root3 = FXMLLoader.load(getClass().getResource("generatwo.fxml"));
        Scene scene2 = new Scene(root3);
        Stage window2 = new Stage();
        window2.setTitle("Voeniy statys");
        window2.setScene(scene2);
        window2.show();
    }

    public void currentStatusINFO(){
        int redCounter = 0;
        int blueCounter = 0;
        for (Scout s : world.getShips()){
            if (s.getSide().equals("Green")){
                redCounter++;
            }
            else{
                blueCounter++;
            }
        }
        if (closePlanets){
            world.getClosePlanetsText().setText("Planets is closed");
        }else{
            world.getClosePlanetsText().setText("Planets is open");
        }

        if (!removeFromWorld){
            world.getRemoveFromWorldText().setText("Died mod is OFF");
        }else{
            world.getRemoveFromWorldText().setText("Died mod is ON");
        }

        if (moveToCoffee){
            world.getMoveToCoffeeText().setText("STO OPEN");
        }else{
            world.getMoveToCoffeeText().setText("STO CLOSED");
        }

        if (stayHome){
            world.getStayHomeText().setText("Stay home");
        }else{
            world.getStayHomeText().setText("Don't stay home");
        }

        world.getRedCounterText().setText("Red ships: " + redCounter);
        world.getBlueCounterText().setText("Blue ships: " + blueCounter);
    }

    public void updateChordINFO(){
        world.getRedCounterText().setX(scrollX + 20);
        world.getRedCounterText().setY(scrollY + 60);

        world.getBlueCounterText().setX(scrollX + 20);
        world.getBlueCounterText().setY(scrollY + 90);

        world.getClosePlanetsText().setX(scrollX + 20);
        world.getClosePlanetsText().setY(scrollY + 120);

        world.getRemoveFromWorldText().setX(scrollX + 20);
        world.getRemoveFromWorldText().setY(scrollY + 150);

        world.getMoveToCoffeeText().setX(scrollX + 20);
        world.getMoveToCoffeeText().setY(scrollY + 180);

        world.getStayHomeText().setX(scrollX + 20);
        world.getStayHomeText().setY(scrollY + 210);
    }



    public static void main(String[] args) {
        launch(args);
    }

}
