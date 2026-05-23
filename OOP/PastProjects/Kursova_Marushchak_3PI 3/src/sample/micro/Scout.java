package sample.micro;

import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import sample.Main;
import sample.World;
import sample.secondMacro.Copy;

import static sample.Main.*;


public class Scout implements Cloneable{
    protected Text name;
    protected String type;
    protected int health;
    protected int damage;
    protected int experience;
    protected int speed;
    protected String side;
    protected double deltaHealth;
    protected boolean isActive;
    protected int nativePlanetX;
    protected int nativePlanetY;
    protected Copy copy;

    protected int baseScoutDamage = 5;
    protected int baseHeavyDamage = 8;
    protected int baseStarShipDamage = 11;



    protected double chordX;
    protected double chordY;

    protected ImageView shipImage;
    protected Group shipGroup;
    protected Rectangle rectActive;
    protected Line lineHealth;
    protected Line lineDamage;

    protected byte autoMove;

    protected boolean isGeneral = false;

    protected boolean isCatch = false;

    protected double currentSoldierX;
    protected double currentSoldierY;

    protected static int numberOfShips = 0;

    //getters
    public Group getShipGroup() {
        return shipGroup;
    }
    public String getType() {
        return type;
    }
    public double getChordX() {
        return chordX;
    }
    public double getChordY() {
        return chordY;
    }
    public boolean getIsActive(){return isActive;}
    public ImageView getShipImage() {
        return shipImage;
    }
    public static int getNumberOfShips() {
        return numberOfShips;
    }
    public int getHealth() {
        return health;
    }
    public int getDamage() {
        return damage;
    }
    public double getDeltaHealth() {
        return deltaHealth;
    }
    public String getSide() {
        return side;
    }
    public Text getName() {
        return name;
    }
    public int getSpeed() {
        return speed;
    }
    public boolean isCatch() {
        return isCatch;
    }
    public boolean isGeneral() {
        return isGeneral;
    }

    //setters
    public void setChordY(double chordY) {
        this.chordY = chordY;
    }
    public void setChordX(double chordX) {
        this.chordX = chordX;
    }
    public static void setNumberOfShips(int numberOfShips) {
        Scout.numberOfShips = numberOfShips;
    }
    public void setHealth(int health) {
        this.health = health;
    }
    public void setActive() {
        this.isActive = !this.isActive;
    }
    public void setNativePlanetX(int nativePlanetX) {
        this.nativePlanetX = nativePlanetX;
    }
    public void setNativePlanetY(int nativePlanetY) {
        this.nativePlanetY = nativePlanetY;
    }
    public void setGeneral(boolean general) {
        isGeneral = general;
    }

    public Scout(){
        numberOfShips++;

        this.copy = new Copy(Main.getNamesArray()[Main.random.nextInt(Main.getNamesArray().length)]);

        this.type = "Scout";
        this.side = "Green";

        this.nativePlanetX = 1000;
        this.nativePlanetY = 1000;

        this.isActive = false;
        this.speed = 5;
        this.health = 500;
        this.damage = 5;
        this.experience = 200;
        this.deltaHealth = 0.2;

        try{
            this.shipImage = new ImageView(new Image("images/scout.png"));
        }catch (Exception e){
            System.out.println("Error!");
        }
        this.shipImage.setFitHeight(100);
        this.shipImage.setPreserveRatio(true);

        this.name= new Text(Main.getNamesArray()[Main.random.nextInt(Main.getNamesArray().length)]);
        this.name.setFont(new Font("Muna", 20));
        this.name.setFill(Color.WHITE);

        this.rectActive = new Rectangle(135,100);
        this.rectActive.setStrokeWidth(3);
        this.rectActive.setStroke(Color.YELLOW);
        this.rectActive.setFill(Color.TRANSPARENT);

        this.lineHealth = new Line();
        this.lineHealth.setStroke(Color.GREEN);
        this.lineHealth.setStrokeWidth(5);

        this.lineDamage = new Line();
        this.lineDamage.setStroke(Color.RED);
        this.lineDamage.setStrokeWidth(5);

        this.autoMove = (byte) Main.random.nextInt(8);

        this.shipGroup = new Group(shipImage, this.name, rectActive, lineHealth, lineDamage);

        shipGroup.addEventHandler(MouseEvent.MOUSE_CLICKED, event ->{
            if (event.getButton() == MouseButton.PRIMARY){
                this.isActive = !this.isActive;

            }

//            if(event.getButton() == MouseButton.SECONDARY){
//                System.out.println("Ім'я: " + this.getName().getText());
//                System.out.println("Рівень" + this.getType());
//                System.out.println("Сторона: " + this.getSide());
//                System.out.println("Активований: " + this.getIsActive());
//                System.out.println("Рівень життя: " + this.getHealth());
//                System.out.println("Рівень сили: " + this.getDamage());
//                System.out.println("Поточна швидкість: " + this.getSpeed());
//                System.out.println("Координати по Х" + this.getChordX());
//                System.out.println("Координати по Y" + this.getChordY());
//            }
        } );

    }

    //делегування конструктора
    public Scout(String name, boolean isActive, int speed, int health, int damage, int experience, String side,  int chordX, int chordY){
        this(name, isActive, speed, health, damage, experience, side);
        this.chordX = chordX;
        this.chordY = chordY;
    }

    public Scout(String name, boolean isActive, int speed, int health, int damage, int experience, String side){
        numberOfShips++;

        this.copy = new Copy(Main.getNamesArray()[Main.random.nextInt(Main.getNamesArray().length)]);

        this.type = "Scout";
        this.isActive = isActive;
        this.speed = speed;
        this.health = health;
        this.damage = damage;
        this.side = side;
        this.experience = experience;
        this.deltaHealth = 0.2;

        if ("Green".equals(this.side)) {
            this.nativePlanetX = 1000;
            this.nativePlanetY = 1000;
        } else {
            this.nativePlanetX = 4000;
            this.nativePlanetY = 2000;
        }

        try{
            if (this.side.equals("Green")){
                this.shipImage = new ImageView(new Image("images/scout.png"));
            }else{
                this.shipImage = new ImageView(new Image("images/scoutSideTwo.png"));
            }
        }catch (Exception e){
            System.out.println("Error!");
        }

        this.shipImage.setFitHeight(100);
        this.shipImage.setPreserveRatio(true);

        this.name = new Text(name);
        this.name.setFont(new Font("Muna", 20));
        this.name.setFill(Color.WHITE);

        this.rectActive = new Rectangle(135,100);
        this.rectActive.setStrokeWidth(3);
        this.rectActive.setStroke(Color.YELLOW);
        this.rectActive.setFill(Color.TRANSPARENT);

        this.lineHealth = new Line();
        this.lineHealth.setStroke(Color.GREEN);
        this.lineHealth.setStrokeWidth(5);

        this.lineDamage = new Line();
        this.lineDamage.setStroke(Color.RED);
        this.lineDamage.setStrokeWidth(5);

        this.autoMove = (byte) Main.random.nextInt(8);

        this.shipGroup = new Group(shipImage, this.name, rectActive, lineHealth, lineDamage);

        shipGroup.addEventHandler(MouseEvent.MOUSE_CLICKED, event ->{
            if (event.getButton() == MouseButton.PRIMARY){
                this.isActive = !this.isActive;

            }
            if(event.getButton() == MouseButton.SECONDARY){
                System.out.println("Ім'я: " + this.getName().getText());
                System.out.println("Рівень" + this.getType());
                System.out.println("Сторона: " + this.getSide());
                System.out.println("Активований: " + this.getIsActive());
                System.out.println("Рівень життя: " + this.getHealth());
                System.out.println("Рівень сили: " + this.getDamage());
                System.out.println("Поточна швидкість: " + this.getSpeed());
                System.out.println("Координати по Х" + this.getChordX());
                System.out.println("Координати по Y" + this.getChordY());
            }
        } );
    }

    public void setShipChord(){
        this.shipImage.setX(this.chordX);
        this.shipImage.setY(this.chordY);

        this.lineHealth.setStartX(this.chordX);
        this.lineHealth.setStartY(this.chordY-10);
        this.lineHealth.setEndX(this.chordX + (this.health * this.deltaHealth));
        this.lineHealth.setEndY(this.chordY-10);

        this.lineDamage.setStartX(this.chordX);
        this.lineDamage.setStartY(this.chordY-20);
        this.lineDamage.setEndX(this.chordX+100);
        this.lineDamage.setEndY(this.chordY-20);

        this.name.setX(this.chordX);
        this.name.setY(this.chordY-30);

        this.rectActive.setX(this.chordX);
        this.rectActive.setY(this.chordY);
        if (this.isActive){
            rectActive.setOpacity(1);
        }
        else {
            rectActive.setOpacity(0);
        }
    }

    public void up(int delta){
        this.chordY -= speed + delta;
        this.shipImage.setRotate(180);
        this.setShipChord();
    }

    public void down(int delta){
        this.chordY += speed + delta;
        this.shipImage.setRotate(0);
        this.setShipChord();
    }

    public void left(int delta){
        this.chordX -= speed + delta;
        this.shipImage.setRotate(90);
        this.setShipChord();
    }

    public void right(int delta){
        this.chordX += speed + delta;
        this.shipImage.setRotate(270);
        this.setShipChord();
    }


    public void ToCoffee(){
        if (Main.getMoveToCoffe()){
            this.nativePlanetX = 3000;
            this.nativePlanetY = 1500;
            this.damage = 0;
        }else{
            if (this.side.equals("Green")){
                this.nativePlanetX = 1000;
                this.nativePlanetY = 1000;
            }else{
                this.nativePlanetX = 4000;
                this.nativePlanetY = 2000;
            }
            switch (this.type){
                case "Scout":
                    this.damage = baseScoutDamage;
                    break;
                case "Heavy":
                    this.damage = baseHeavyDamage;
                    break;
                case "StarShip":
                    this.damage = baseStarShipDamage;
            }
        }
    }



    public void moveToPlanet(){
        if (!this.isActive){
            if (nativePlanetX + 40 < this.chordX){
                this.shipImage.setRotate(90);
                    this.chordX -= this.speed + Main.getDeltaSpeed();
            }
            if (nativePlanetX + 40 > this.chordX){
                this.shipImage.setRotate(270);
                    this.chordX += this.speed + Main.getDeltaSpeed();
            }
            if (nativePlanetY + 40 > this.chordY){
                this.shipImage.setRotate(0);
                    this.chordY += this.speed + Main.getDeltaSpeed();
            }
            if (nativePlanetY + 40 < this.chordY){
                this.shipImage.setRotate(180);
                    this.chordY -= this.speed + Main.getDeltaSpeed();
            }
        }

    }

    public void healthUpGreen(){
        if (this.side.equals("Green")){
            this.shipGroup.setOpacity(0);
            if (this.health * this.deltaHealth < 100){
                this.health+=4;
            }
        }
    }

    public void healthUpRed(){
        if (this.side.equals("Red")){
            this.shipGroup.setOpacity(0);
            if (this.health * this.deltaHealth < 100){
                this.health+=4;
            }
        }
    }

    public void healthUpMoon(){
        this.shipGroup.setOpacity(0);
        if (this.health * this.deltaHealth < 100)
            this.health+=4;
    }


    public void autoMove(){
        this.shipGroup.setOpacity(1);
        if (!this.isActive){
            switch (this.autoMove){
                case 0:
                    this.chordY -= this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(180);
                    break;
                case 1:
                    this.chordY -= this.speed + Main.getDeltaSpeed();
                    this.chordX += this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(225);
                    break;
                case 2:
                    this.chordX += this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(270);
                    break;
                case 3:
                    this.chordX += this.speed + Main.getDeltaSpeed();
                    this.chordY += this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(315);
                    break;
                case 4:
                    this.chordY += this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(0);
                    break;
                case 5:
                    this.chordY += this.speed + Main.getDeltaSpeed();
                    this.chordX -= this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(45);
                    break;
                case 6:
                    this.chordX -= this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(90);
                    break;
                case 7:
                    this.chordX -= this.speed + Main.getDeltaSpeed();
                    this.chordY -= this.speed + Main.getDeltaSpeed();
                    this.shipImage.setRotate(135);
                    break;
            }
            if (this.shipImage.getX() + this.shipImage.getFitWidth() + 100 >= World.getRoot().getWidth() - this.shipImage.getFitWidth()){
                this.autoMove = (byte) (Main.random.nextInt(3) + 5);
            }else if (this.shipImage.getX() <= 0){
                this.autoMove = (byte) (Main.random.nextInt(3) + 1);
            }else if (this.shipImage.getY() <= 0){
                this.autoMove = (byte) (Main.random.nextInt(3) + 3);
            }else if (this.shipImage.getY() + this.shipImage.getFitHeight() >= World.getRoot().getHeight()){
                this.autoMove = (byte)  (Main.random.nextInt(2));
            }
        }
    }

    public static void shipInteraction(Scout ship1, Scout ship2){
        if (ship1.isGeneral || ship2.isGeneral){
            if (ship1.isGeneral){
                isCheack = false;
                ship2.isCatch = true;
                //ship2.getShipGroup().getChildren().removeAll(ship2.shipImage, ship2.lineHealth);
            }else{
                isCheack = false;
               // ship1.getShipGroup().getChildren().removeAll(ship1.shipImage, ship1.lineHealth);
                ship1.isCatch = true;
            }
        }
//        else if (!ship1.side.equals(ship2.side)){
//            if (ship1.getHealth() >= 0 && ship2.getHealth() >= 0){
//                ship1.setHealth(ship1.getHealth() - ship2.getDamage());
//            }
//            if (ship2.getHealth() >= 0 && ship1.getHealth() >= 0){
//                ship2.setHealth(ship2.getHealth() - ship1.getDamage());
//            }
//        }
    }

    public void sayHello(){
        System.out.print(this.type + "   ");
        System.out.print(this.name.getText() + ":");
        System.out.println(" Hello");
    }

    public void setGeneral(){
        this.getShipGroup().getChildren().removeAll(shipImage, lineDamage, lineHealth, name);
        this.name = new Text("ПЕТРОВИЧ");
        this.speed = 22;
        this.damage = 0;
        this.isGeneral = true;
        this.shipImage = new ImageView(new Image("images/General.png"));
        this.name.setFont(new Font("Monaco", 30));
        this.name.setFill(Color.WHITE);
        this.shipGroup.getChildren().addAll(shipImage, name);
    }

    public void moveToSoldier(){
        updateSoldierChord();
        this.shipGroup.setOpacity(1);
        if (!this.isActive){
            if (nextX + 40 < this.chordX){
                //this.shipImage.setRotate(90);
                this.chordX -= this.speed + Main.getDeltaSpeed();
            }
            if (nextX + 40 > this.chordX){
                //this.shipImage.setRotate(270);
                this.chordX += this.speed + Main.getDeltaSpeed();
            }
            if (nextY + 40 > this.chordY){
                //this.shipImage.setRotate(0);
                this.chordY += this.speed + Main.getDeltaSpeed();
            }
            if (nextY + 40 < this.chordY){
                //this.shipImage.setRotate(180);
                this.chordY -= this.speed + Main.getDeltaSpeed();
            }
        }
    }

    public void updateSoldierChord(){
        if (!Main.isCheack){
            for (Scout s : Main.getWorld().getShips()){
                if (!s.isGeneral && s.isCatch == false){
                    buffScout = s;
                    Main.nextX = s.chordX;
                    Main.nextY = s.chordY;
                    Main.isCheack = !isCheack;
                }
            }
        }
        Main.nextX = buffScout.chordX;
        Main.nextY = buffScout.chordY;

    }


    public void moveLeft(double x, double y, Scout s){
        this.chordX = x;
        this.chordY = y;
        this.shipImage.setRotate(270);

      for (int i = 0; i<Main.getWorld().getShips().size(); i++){
          if (Main.getWorld().getShips().get(i) == s){
              Main.getWorld().getShips().remove(i);
          }
      }
      heavyCounter++;
    }

    public void moveDown(double x, double y, Scout s){
        this.chordX = x;
        this.chordY = y;
        this.shipImage.setRotate(180);

        for (int i = 0; i<Main.getWorld().getShips().size(); i++){
            if (Main.getWorld().getShips().get(i) == s){
                Main.getWorld().getShips().remove(i);
            }
        }
        scoutCounter++;
    }

    public void moveUP(double x, double y, Scout s){
        this.chordX = x;
        this.chordY = y;
        this.shipImage.setRotate(270);

        for (int i = 0; i<Main.getWorld().getShips().size(); i++){
            if (Main.getWorld().getShips().get(i) == s){
                Main.getWorld().getShips().remove(i);
            }
        }
        starShipCounter++;
    }


    @Override
    protected Scout clone() throws CloneNotSupportedException {
        Scout newScout = (Scout) super.clone();
        newScout.copy = (Copy) copy.clone();
        return newScout;
    }
}
