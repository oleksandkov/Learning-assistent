package sample;

import army.Army;
import army.window.EnterNumbersWindow;
import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Bounds;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.ScrollPane;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.stage.Stage;
import objects.firstMacro.Guitar;
import objects.firstMacro.Piano;
import objects.firstMacro.Trembita;
import objects.firstMacro.Violin;
import objects.micro.MusicianMaster;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import objects.secondMacro.Factory;
import objects.secondMacro.School;
import objects.secondMacro.Shop;
import objects.secondMacro.Underpass;
import objects.thirdMacro.City;

import java.io.IOException;
import java.util.Arrays;
import java.util.Date;
import java.util.Random;

public class Main extends Application {


    final public static int SCENE_WIDTH = 1920;
    final public static int SCENE_HEIGHT = 1080;

    public static Random random = new Random(new Date().getTime());

    private static City city = new City();
    private static ScrollPane scrollPane = new ScrollPane(city.getRoot());
    private static Scene scene = new Scene(scrollPane, Main.SCENE_WIDTH, Main.SCENE_HEIGHT);

    private static double scrollX;
    private static double scrollY;

    public static City getCity() {
        return city;
    }
    public static ScrollPane getScrollPane() {
        return scrollPane;
    }
    public static Scene getScene() {
        return scene;
    }

    public static double getScrollX() {
        return scrollX;
    }
    public static double getScrollY() {
        return scrollY;
    }


    @Override
    public void start(Stage primaryStage) throws Exception {

        scrollPane.setFitToHeight(true);
        scrollPane.setFitToWidth(true);

        city.addNewBuilding(new Factory());
        city.addNewBuilding(new Factory());
        city.addNewBuilding(new Shop());
        city.addNewBuilding(new Shop());
        city.addNewBuilding(new School());
        city.addNewBuilding(new School());
        city.addNewBuilding(new Underpass());
        city.addNewBuilding(new Underpass());

        city.addNewShopper(new Shopper(new Violin(),false, "Maks", 100, true),true);
        city.addNewShopper(new Shopper(new Trembita(),false, "Pasha", 50, true),true);
        city.addNewShopper(new Shopper(new Guitar(),false, "Dima", 149, true),true);
        city.addNewShopper(new Shopper(new Guitar(), false, "Sanya", 1000, true),true);
        city.addNewShopper(new Shopper(new Guitar(), false, "Sanya", 1000, true),true);
        city.addNewShopper(new MusicianMaster(new Guitar(), false, "Sanya", 1000, true),true);
        city.addNewShopper(new MusicianMaster(new Guitar(), false, "Sanya", 1000, true),true);
        city.addNewShopper(new MusicianMaster(new Guitar(), false, "Sanya", 1000, true),true);
        city.addNewShopper(new MusicianMaster(new Guitar(), false, "Sanya", 1000, true),true);

        city.addNewShopper(new OrchestraConductor(new Guitar(), false, "Sanya", 1000),true);
        city.addNewShopper(new OrchestraConductor(new Guitar(), false, "Sanya", 1000),true);
        city.addNewShopper(new OrchestraConductor(new Guitar(), false, "Sanya", 1000),true);
        city.addNewShopper(new OrchestraConductor(new Guitar(), false, "Sanya", 1000),true);


        Parent parent = FXMLLoader.load(getClass().getResource("sample.fxml"));
        city.getRoot().getChildren().add(parent);


        scrollPane.viewportBoundsProperty().addListener(new ChangeListener<Bounds>() {
            @Override
            public void changed(ObservableValue<? extends Bounds>
                                        observable, Bounds oldBounds,
                                Bounds bounds) {
                double scrollWidth;
                double scrollHeight;
                Main.scrollX = -1 * (int) bounds.getMinX();
                scrollWidth = -1 * (int) bounds.getMinX() + (int) bounds.getWidth();
                Main.scrollY = -1 * (int) bounds.getMinY();
                scrollHeight = -1 * (int) bounds.getMinY() + bounds.getHeight();

                //постійний здвиг стрічки меню при прокручуванні
                parent.setLayoutX(scrollX);
                parent.setLayoutY(scrollY);

                // постійни здвиг карти при прокручуванні
                city.getMiniMap().getPane().setLayoutX(scrollX + 10);
                city.getMiniMap().getPane().setLayoutY(scrollY + scene.getHeight() - city.getMiniMap().getPane().getHeight() - 25);
                city.getMiniMap().getMainArea().setLayoutX(scrollX*MiniMap.getSCALE());
                city.getMiniMap().getMainArea().setLayoutY(scrollY*MiniMap.getSCALE());

                city.getInformationGroup().setLayoutX(scrollX);
                city.getInformationGroup().setLayoutY(scrollY);
               /* city.getInteractWithPlayerModeLabel().setLayoutX(scrollX+10);
                city.getInteractWithPlayerModeLabel().setLayoutY(scrollY+scene.getHeight()-city.getMiniMap().getPane().getHeight() - 50);
*/
                /*//просто показує координати в даний момент
                System.out.println(" X from " + Main.scrollX + " to " +
                        scrollWidth + "; Y from " + Main.scrollY + " to " +
                        scrollHeight);*/
            }
        });

        primaryStage.setMaximized(true);
        primaryStage.setScene(scene);
        primaryStage.setTitle("Let's play this instrument!");
        primaryStage.getIcons().add(new Image("assets/icon.png"));
        scene.setOnKeyPressed(new KeyPressedHandler());

        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.freeRun();
                    shopper.setShopperInChords();
                }
                city.getMiniMap().updateMap();
                city.updateInformationLabel();
                if (Army.checkArmyFinish()){
                    Army.setArmy(false);
                    for(Shopper shopper: Main.getCity().getShoppers()){
                        Army.N3ActiveReady=0;
                        Army.N3Ready=0;
                        Army.N2ActiveReady=0;
                        Army.N2Ready = 0;
                        Army.N1ActiveReady=0;
                        Army.N1Ready=0;
                        shopper.setInArmy(false);
                        shopper.setxStay(false);
                        shopper.setyStay(false);
                        shopper.setNumberInRank(0);
                    }
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setContentText("Призив завершився на 100%\nМожете грати далі");
                    alert.show();
                }
            }
        };
        timer.start();
        primaryStage.show();
    }

    private class KeyPressedHandler implements EventHandler<KeyEvent> {

        //З ПИТАННЯМИ, ЧОМУ ТУТ ПОЛОВИНА КОДУ ВИКЛЮЧЕНА, ЗВЕРТАТИСЯ ДО МЕНЕ!!!
        @Override
        public void handle(KeyEvent event) {

            //ПРИЗОВ
            if (event.getCode().equals(KeyCode.P)){
                try {
                    new EnterNumbersWindow().display();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            /*if (event.getCode().equals(KeyCode.M)) {
                try {
                    new CreateShopperWindow().display("Let's create new Shopper!");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (event.getCode().equals(KeyCode.DELETE)) {
                for (int i = 0; i < shoppers.size(); i++) {
                    Shopper shopper = shoppers.get(i);
                    if (shopper.isActive()) {
                        Main.deleteAShopper(shopper);
                        i--;
                    }

                }
                System.out.println(Shopper.getNumberOfShoppers() + "----");
                System.out.println(shoppers.size());
            }
            if (event.getCode().equals(KeyCode.ESCAPE)) {
                for (Shopper shopper : shoppers) {
                    if (shopper.isActive()) {
                        shopper.setActive(false);
                    }
                }
            }
            if (event.getCode().equals(KeyCode.J)) {
                for (Shopper shopper : shoppers) {
                    if (shopper.isActive()) {
                        for (Building b : buildings) {
                            b.interact(shopper, event.isShiftDown());
                        }
                    }
                }
            }
            if (event.getCode().equals(KeyCode.K)) {
                for (Shopper shopper : shoppers) {
                    if (shopper.isActive() && shopper.getInstrument() != null) {
                        shopper.playAnInstrument();
                        break;
                    }
                }
            }*/
            if (event.getCode().equals(KeyCode.X)){
                Shopper[] shoppersArray = Main.getCity().getShoppers().toArray(new Shopper[0]);
                System.out.println("Відсортовані за кількістю грошей:");
                Arrays.sort(shoppersArray, (o1,o2) -> (int) (o1.getMoney()-o2.getMoney()));

                for (Shopper shopper: shoppersArray){
                    System.out.println(shopper.getName());
                }
                System.out.println("\nВідсортовані за ім'ям:");
                Arrays.sort(shoppersArray, (o1, o2) -> o1.getName().compareTo(o2.getName()));
                for (Shopper shopper: shoppersArray){
                    System.out.println(shopper.getName());
                }
                System.out.println(Arrays.toString(shoppersArray));
            }
            if (event.getCode().equals(KeyCode.W) && !event.isShiftDown()) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.up(1);
                }
            } else if (event.getCode().equals(KeyCode.S) && !event.isShiftDown()) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.down(1);
                }
            } else if (event.getCode().equals(KeyCode.D) && !event.isShiftDown()) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.right(1);
                }
            } else if (event.getCode().equals(KeyCode.A) && !event.isShiftDown()) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.left(1);
                }
            } else if (event.getCode().equals(KeyCode.W)) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.up(2);
                }
            } else if (event.getCode().equals(KeyCode.S)) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.down(2);
                }
            } else if (event.getCode().equals(KeyCode.D)) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.right(2);
                }
            } else if (event.getCode().equals(KeyCode.A)) {
                for (Shopper shopper : city.getShoppers()) {
                    shopper.left(2);
                }
            }
        }
    }


    public static void main(String[] args) {
        launch(args);
    }
}
