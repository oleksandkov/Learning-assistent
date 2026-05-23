package objects.thirdMacro;

import army.Army;
import javafx.scene.Group;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import objects.micro.MusicianMaster;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import objects.secondMacro.*;
import sample.Main;
import sample.MiniMap;
import sample.windows.preferencesWindow.Preferences;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.function.Consumer;

public class City {
    final public static int ROOT_WIDTH = 4800;
    final public static int ROOT_HEIGHT = 2700;
    private boolean interactWithPlayerMode; //характер руху

    private Label informationLabel;
    private Rectangle informationRectangle;
    private Group informationGroup;
    private Button informationButton;

    private Pane root;
    private ArrayList<Shopper> shoppers;
    private ArrayList<Building> buildings;
    private HashMap<String,ArrayList> objectsHashMap = new HashMap<>(); // для більш оптимізованої взаємодії мікрооб'єктів з макро
    private MiniMap miniMap;


    public City(){
        this.root = new Pane();
        this.root.setMinWidth(City.ROOT_WIDTH);
        this.root.setMinHeight(City.ROOT_HEIGHT);
        this.shoppers = new ArrayList<>(5);
        this.buildings = new ArrayList<>(5);
        root.setOnMousePressed(event -> {
            if (Army.isArmy()){
                Army.toArmy(event.getX(),event.getY());
            } else {
                for (Shopper shopper : this.shoppers) {
                    if (event.getButton().equals(MouseButton.PRIMARY)) {
                        shopper.mouseClick(event.getX(), event.getY());
                    } else if (event.getButton().equals(MouseButton.SECONDARY)) {
                        if (shopper instanceof OrchestraConductor) {
                            ((OrchestraConductor) shopper).chooseAnInstrument(event.getX(), event.getY());
                        }
                    }
                }
            }
        });

        this.interactWithPlayerMode = true;
        this.informationLabel = new Label();
        this.informationLabel.setFont(new Font("Segoe UI Black Italic", 15));
        this.informationLabel.setLayoutX(20);
        this.informationLabel.setLayoutY(80);

        this.informationRectangle = new Rectangle(480,250);
        this.informationRectangle.setFill(Color.LIGHTGREY);
        this.informationRectangle.setStroke(Color.BLACK);
        this.informationRectangle.setLayoutX(10);
        this.informationRectangle.setLayoutY(70);

        this.informationButton = new Button("Ігрова інформація ▼");
        this.informationButton.setFont(new Font("Segoe UI Black Italic", 15));
        this.informationButton.setMinWidth(480);
        this.informationButton.setMinHeight(15);
        this.informationButton.setLayoutX(10);
        this.informationButton.setLayoutY(35);
        this.informationButton.setOnAction(event -> {
            if (this.informationRectangle.getOpacity()==1){
                this.informationRectangle.setOpacity(0);
                this.informationLabel.setOpacity(0);
                this.informationButton.setText("Ігрова інформація ▲");
            }else {
                this.informationRectangle.setOpacity(1);
                this.informationLabel.setOpacity(1);
                this.informationButton.setText("Ігрова інформація ▼");
            }
        });

        this.informationGroup = new Group(informationRectangle,informationLabel, informationButton);
        this.root.getChildren().addAll(informationGroup);

        this.miniMap = new MiniMap();
        this.root.getChildren().addAll(miniMap.getPane());

        //опасно так робити))))
        objectsHashMap.put("Factory", new ArrayList<Factory>());
        objectsHashMap.put("Shop", new ArrayList<Shop>());
        objectsHashMap.put("School", new ArrayList<School>());
        objectsHashMap.put("Underpass", new ArrayList<Underpass>());
        objectsHashMap.put("Shopper", new ArrayList<Shopper>());
        objectsHashMap.put("Master", new ArrayList<MusicianMaster>());
        objectsHashMap.put("Orchestra", new ArrayList<OrchestraConductor>());
    }


    public void addNewShopper(Shopper shopper, boolean rand) {
        this.shoppers.add(shopper);
        if (rand) {
            shopper.setXChord(Main.random.nextInt((int) (Main.getScene().getWidth() + Main.getScrollX())));
            shopper.setYChord(Main.random.nextInt((int) (Main.getScene().getHeight() + Main.getScrollY())));
        }
        shopper.setShopperInChords();
        this.root.getChildren().add(Building.getNumberOfBuildings(),shopper.getShopperPicture());
        this.miniMap.addShopper(shopper);

        this.objectsHashMap.get(shopper.getType()).add(shopper);
    }

    public void deleteAShopper(Shopper shopper) {
        this.miniMap.deleteAShopper(shopper);
        this.root.getChildren().remove(shopper.getShopperPicture());
        this.shoppers.remove(shopper);
        Shopper.setNumberOfShoppers(Shopper.getNumberOfShoppers() - 1);
        this.objectsHashMap.get(shopper.getType()).remove(shopper);

    }

    public void addNewBuilding(Building building) {
        this.buildings.add(building);
        building.setBuildingInChords();
        this.root.getChildren().add(0,building.getBuildingPicture());
        this.miniMap.addBuilding(building);

        this.objectsHashMap.get(building.getType()).add(building);

    }

    public void deleteABuilding(Building building) {
        this.miniMap.deleteABuilding(building);
        this.root.getChildren().remove(building.getBuildingPicture());
        this.buildings.remove(building);
        Building.setNumberOfBuildings(Building.getNumberOfBuildings() - 1);

        this.objectsHashMap.get(building.getType()).remove(building);
    }

    public void updateInformationLabel(){
        if (Preferences.isINFORMATION()){
            this.getInformationGroup().setOpacity(1);
        }else {
            this.getInformationGroup().setOpacity(0);
        }
        String mode = interactWithPlayerMode ? "взаємодія з користувачем (N)" : "вільна взаємодія (N)";
        int numberOfActive = 0;
        int numberWithoutInstrument = 0;

        int numberOfShoppersInsideUnderpass = objectsHashMap.get("Underpass").stream().mapToInt(o -> ((Underpass) o).getShoppersInsideSize()).sum();
        for (Shopper shopper: shoppers){
            if (shopper.isActive()){
                numberOfActive++;
            }
            if (shopper instanceof OrchestraConductor){
                if (((OrchestraConductor) shopper).getInstruments().size()==0)
                    numberWithoutInstrument++;
            } else if (shopper.getInstrument()==null){
                numberWithoutInstrument++;
            }


        }
        this.informationLabel.setText("-режим руху: "+mode+"\n-кількість вуличних музикантів: "+objectsHashMap.get("Shopper").size()+"\n-кількість випускників муз. школи: "+objectsHashMap.get("Master").size()+"\n-кількість диригентів: "+objectsHashMap.get("Orchestra").size()+"\n-активних мікрооб'єктів: "+numberOfActive+"\n-кількість мікрооб'єктів без інструменту: "+numberWithoutInstrument+"\n-кількість мікрооб'єктів всередині підземних переходів: "+numberOfShoppersInsideUnderpass+"\n-кількість фабрик: "+objectsHashMap.get("Factory").size()+"\n-кількість музичкних шкіл: "+objectsHashMap.get("School").size()+"\n-кількість магазинів: "+objectsHashMap.get("Shop").size()+"\n-кількість підземних переходів: "+objectsHashMap.get("Underpass").size());
    }

    public Pane getRoot() {
        return root;
    }

    public ArrayList<Shopper> getShoppers() {
        return shoppers;
    }

    public ArrayList<Building> getBuildings() {
        return buildings;
    }

    public MiniMap getMiniMap() {
        return miniMap;
    }

    public boolean isInteractWithPlayerMode() {
        return interactWithPlayerMode;
    }

    public void setInteractWithPlayerMode(boolean interactWithPlayerMode) {
        this.interactWithPlayerMode = interactWithPlayerMode;
    }
    public HashMap<String, ArrayList> getObjectsHashMap() {
        return objectsHashMap;
    }

    public Group getInformationGroup() {
        return informationGroup;
    }
}