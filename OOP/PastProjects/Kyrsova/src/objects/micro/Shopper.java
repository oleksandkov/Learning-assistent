package objects.micro;


import army.Army;
import javafx.animation.Animation;
import javafx.animation.AnimationTimer;
import javafx.geometry.Point2D;
import javafx.scene.Group;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.effect.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.media.AudioClip;
import javafx.scene.media.Media;
import javafx.scene.media.MediaException;
import javafx.scene.media.MediaPlayer;
import javafx.scene.paint.Color;
import javafx.scene.shape.Ellipse;
import javafx.scene.shape.Polygon;
import javafx.scene.text.Font;
import javafx.scene.transform.Rotate;
import javafx.util.Duration;
import objects.firstMacro.Instrument;
import objects.secondMacro.Building;
import sample.Main;
import sample.Sprite;
import sample.windows.preferencesWindow.Preferences;

import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;


public class Shopper implements Cloneable{

    protected double playVariable;
    protected String type;
    protected double xChord, yChord;
    protected byte startDirection;

    protected Ellipse shadow;

    protected boolean isActive;
    protected boolean stay;
    protected Polygon triangleAct;


    protected ImageView shopperImage;
    protected boolean isMan;
    protected Label shopperText;

    protected Instrument instrument = null;
    protected Group shopperPicture;


    protected double money;
    protected String name;
    protected static int numberOfShoppers = 0;


    protected boolean inArmy = false;
    protected int numberInRank;
    protected boolean xStay= false, yStay =false;

    public Shopper(Instrument instrument, boolean isActive, String name, double money, boolean isMan) {
        Shopper.numberOfShoppers++;
        this.type = "Shopper";
        this.playVariable = 0.1;
        this.name = name;
        this.money = money;
        this.isMan = isMan;
        this.startDirection = (byte) Main.random.nextInt(8);


        if (isMan) {
            this.shopperImage = new ImageView(new Image("assets/shopper.png"));
        } else this.shopperImage = new ImageView(new Image("assets/girlShopper.png"));

        this.isActive = isActive;
        this.stay = false;
        this.shopperImage.setPreserveRatio(true);
        this.shopperImage.setFitHeight(190);
        this.shopperImage.setEffect(new DropShadow(30, 0, 0, Color.GRAY));

        this.instrument = instrument;

        this.shopperText = new Label(/*this.name + ", money: " + this.money*/);
        this.shopperText.setFont(new Font("Segoe UI Black Italic", 13));

        this.shadow = new Ellipse(75, 20);
        this.shadow.setFill(Color.BLACK);
        this.shadow.setOpacity(0.8);
        this.shadow.getTransforms().add(new Rotate(15));
        this.shadow.setEffect(new GaussianBlur(40));

        this.triangleAct = new Polygon();
        triangleAct.getPoints().addAll(0.0, 0.0,
                30.0, 0.0,
                15.0, 25.0);
        this.triangleAct.setFill(Color.BLACK);

        if (this.instrument != null) {
            this.shopperPicture = new Group(shopperImage, shopperText, shadow, triangleAct, this.instrument.getInstrumentImage());
        } else {
            this.shopperPicture = new Group(shopperImage, shopperText, shadow, triangleAct);
        }
    }

    public Shopper(boolean isActive, String name, double money, boolean isMan) {
        this(null, isActive, name, money, isMan);
    }

    public Shopper(Instrument instrument, boolean isActive, String name) {
        this(instrument, isActive, name, 0, true);
    }

    public Shopper(boolean isActive, String name) {
        this(null, isActive, name, 0, true);
    }

    public Shopper(boolean isActive, String name, double money) {
        this(null, isActive, name, money, true);
    }

    public Shopper(String name) {
        this(null, true, name, 0, true);
    }

    public Shopper() {
        this(null, false, null, 0, true);
    }


    public void setShopperInChords() {
        this.shopperImage.setX(this.xChord);
        this.shopperImage.setY(this.yChord);

        this.shopperText.setLayoutX(this.xChord - 15);
        this.shopperText.setLayoutY(this.yChord - 25);
        this.shopperText.setText(this.name + ", money: " + this.money);

        this.shadow.setLayoutX(this.xChord - 10);
        this.shadow.setLayoutY(this.yChord + 170);

        this.triangleAct.setLayoutX(this.xChord + 30);
        this.triangleAct.setLayoutY(this.yChord - 40);

        if (this.isActive) {
            this.triangleAct.setOpacity(1);
        } else {
            this.triangleAct.setOpacity(0);
        }
        if (this.instrument != null) {
            this.instrument.update(this.xChord, this.yChord);
        }
    }

    public void freeRun() {
        if (this.isInArmy()){
            this.moveToArmy();
        } else {
            if (!this.isActive() && !this.stay) {

                double speed = Main.getCity().isInteractWithPlayerMode() ? Preferences.getSPEED() / 3 : Preferences.getSPEED();
                if ((this instanceof OrchestraConductor) && !Main.getCity().isInteractWithPlayerMode() && Main.getCity().getShoppers().size() >= 2)
                    speed = 0;
                switch (this.startDirection) {
                    case 0:
                        this.yChord -= speed;
                        break;
                    case 1:
                        this.yChord -= speed;
                        this.xChord += speed;
                        break;
                    case 2:
                        this.xChord += speed;
                        break;
                    case 3:
                        this.xChord += speed;
                        this.yChord += speed;
                        break;
                    case 4:
                        this.yChord += speed;
                        break;
                    case 5:
                        this.yChord += speed;
                        this.xChord -= speed;
                        break;
                    case 6:
                        this.xChord -= speed;
                        break;
                    case 7:
                        this.xChord -= speed;
                        this.yChord -= speed;
                        break;
                }
                if (Main.getCity().isInteractWithPlayerMode()) {
                    if (this.shopperImage.getX() + 100 >= Main.getScene().getWidth() + Main.getScrollX()) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 5);
                    } else if (this.shopperImage.getX() <= 0 + Main.getScrollX()) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 1);
                    } else if (this.shopperImage.getY() <= 0 + Main.getScrollY()) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 3);
                    } else if (this.shopperImage.getY() + 170 >= Main.getScene().getHeight() + Main.getScrollY()) {
                        this.startDirection = (byte) Main.random.nextInt(2);
                    }
                } else {
                    if (this.shopperImage.getX() + 100 >= Main.getCity().getRoot().getWidth()) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 5);
                    } else if (this.shopperImage.getX() <= 0) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 1);
                    } else if (this.shopperImage.getY() <= 0) {
                        this.startDirection = (byte) (Main.random.nextInt(3) + 3);
                    } else if (this.shopperImage.getY() + 170 >= Main.getCity().getRoot().getHeight()) {
                        this.startDirection = (byte) Main.random.nextInt(2);
                    }
                    this.freeInteract();
                }
            }
        }
    }

    public void mouseClick(double x, double y) {
        if (this.shopperImage.getBoundsInParent().contains(new Point2D(x, y)) && !this.stay) {
            this.isActive = !this.isActive;
        }
    }

    public void left(double boost) {
        if (isActive && xChord >= 0) {
            xChord -= Preferences.getSPEED() * boost;
        }
    }

    public void right(double boost) {
        if (isActive && xChord <= Main.getCity().getRoot().getWidth() - 100) {
            xChord += Preferences.getSPEED() * boost;
        }
    }

    public void up(double boost) {
        if (isActive && yChord >= 0) {
            yChord -= Preferences.getSPEED() * boost;
        }
    }

    public void down(double boost) {
        if (isActive && yChord <= Main.getCity().getRoot().getHeight() - 200) {
            yChord += Preferences.getSPEED() * boost;
        }
    }

    public void setStay(boolean stay) {
        this.isActive = false;
        this.stay = stay;
    }

    public boolean isStay() {
        return stay;
    }

    public void buyAnInstrument(String typeOfInstrument) {
        if (this.money >= Instrument.getInstrument(typeOfInstrument).getPrice()) {
            this.instrument = Instrument.getInstrument(typeOfInstrument);
            this.money -= this.instrument.getPrice();
            this.shopperPicture.getChildren().add(this.instrument.getInstrumentImage());
        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Увага!");
            alert.setHeaderText("Приходьте пізніше)");
            alert.setContentText("Недостатньо коштів для покупки даного інструмента, прямуйте заробляти");
            alert.showAndWait();
        }
    }

    public void sellAnInstrument() {
        if (this.getInstrument() != null) {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setHeaderText("Приходьте пізніше");
            alert.setContentText("Будучи початківцев, продавати ваш єдиний інструмент дуже ризиковано.");
            alert.showAndWait();
        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("У вас немає що продавати");
            alert.showAndWait();
        }
    }

    public Shopper education(boolean first, boolean second, double allPrice) {
        if (first && second && this.money >= allPrice) {// провырку на жынку не забути
            if (this.isMan) {
                return new OrchestraConductor(Instrument.getInstrument(this.instrument.getType()), true, this.name, this.money - allPrice);
            } else {
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setContentText("Жінки не можуть бути дирижорами");
                alert.showAndWait();
            }
        } else if (first && !second && this.money >= allPrice) {
            return new MusicianMaster(Instrument.getInstrument(this.instrument.getType()), true, this.name, this.money - allPrice, this.isMan);
        } else if (second && !first && this.money >= allPrice) {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("Спочатку потрібно пройти початкове навчання, а вже потім учитися на дирижора, або ж можна на обох одразу");
            alert.showAndWait();
        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("Недостаньо коштів");
            alert.showAndWait();
        }
        return this;
    }

    public void playAnInstrument() {

        this.shopperImage.setOpacity(0);

        ImageView shopperPlaySprite;
        if (this.isMan) {
            shopperPlaySprite = new ImageView(new Image("assets/shopperPlaySprite.png"));
        } else {
            shopperPlaySprite = new ImageView(new Image("assets/girlShopperPlaySprite.png"));

        }
        shopperPlaySprite.setX(this.shopperImage.getX());
        shopperPlaySprite.setY(this.shopperImage.getY());
        shopperPlaySprite.setPreserveRatio(true);
        shopperPlaySprite.setFitHeight(this.shopperImage.getFitHeight());


        this.getShopperPicture().getChildren().add(4, shopperPlaySprite);
        this.setStay(true);

        Animation playAnimation = new Sprite(shopperPlaySprite, Duration.millis(2000), 4, 4, 431, 683);
        playAnimation.setCycleCount(10);//як довго буде грати

        String musicPath = "";
        switch (this.instrument.getType()) {
            case "Guitar":
                musicPath = "src/assets/music/witcher.mp3";
                break;
            case "Bayan":
                musicPath = "src/assets/music/bayanBad.mp3";
                break;
            case "Drums":
                musicPath = "src/assets/music/drumsBad.mp3";
                break;
            case "Piano":
                musicPath = "src/assets/music/dogWaltz.mp3";
                break;
            case "Trembita":
                musicPath = "src/assets/music/trembitaBad.mp3";
                break;
            case "Violin":
                musicPath = "src/assets/music/violinBad.mp3";
                break;
        }

        MediaPlayer mediaPlayer = null;
        try {
            //System.out.println(Paths.get(musicPath).toUri().toString());
            Media hit = new Media(Paths.get(musicPath).toUri().toString());
            mediaPlayer = new MediaPlayer(hit);
            mediaPlayer.setVolume(Preferences.getVOLUME());
            mediaPlayer.play();
        } catch (MediaException m) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setHeaderText("Помилка загрузки аудіо!!");
            alert.setContentText(m.toString());
            alert.show();
        }
        MediaPlayer finalMediaPlayer = mediaPlayer;

        playAnimation.play();

        playAnimation.setOnFinished(event -> {
            this.shopperImage.setOpacity(1);
            this.getShopperPicture().getChildren().remove(shopperPlaySprite);
            this.setStay(false);
            if (finalMediaPlayer != null) {
                finalMediaPlayer.stop();
            }
        });

    }
    public void freeInteract(){
        if (this.instrument != null && this.money<200){
            for (Building b : (ArrayList<Building>) Main.getCity().getObjectsHashMap().get("Underpass")) {
                if (b.interact(this, false))
                    break;
            }
        }
        if (this.isMan && this.money<500) {
            for (Building b : (ArrayList<Building>) Main.getCity().getObjectsHashMap().get("Factory")) {
                if (b.interact(this, false))
                    break;
            }
        }
        if (this.instrument != null){
            if (Main.random.nextDouble()>0.99995){
                this.playAnInstrument();
            }
        }
    }

    public double getPlayVariable() {
        return playVariable;
    }

    public String getType() {
        return type;
    }

    public double getXChord() {
        return xChord;
    }

    public void setXChord(double xChord) {
        this.xChord = xChord;
    }

    public double getYChord() {
        return yChord;
    }

    public void setYChord(double yChord) {
        this.yChord = yChord;
    }

    public byte getStartDirection() {
        return startDirection;
    }

    public void setStartDirection(byte startDirection) {
        this.startDirection = startDirection;
    }

    public boolean isActive() {
        return isActive;
    }

    public void setActive(boolean active) {
        isActive = active;
    }

    public ImageView getShopperImage() {
        return shopperImage;
    }

    public boolean isMan() {
        return isMan;
    }

    public void setMan(boolean man) {
        isMan = man;
        if (man)
            this.shopperImage.setImage(new Image("assets/shopper.png"));
        else this.shopperImage.setImage(new Image("assets/girlShopper.png"));
    }

    public Instrument getInstrument() {
        return instrument;
    }

    public void setInstrument(Instrument instrument) {
        this.instrument = instrument;
        this.shopperPicture.getChildren().add(this.instrument.getInstrumentImage());
    }

    public Group getShopperPicture() {
        return shopperPicture;
    }

    public double getMoney() {
        return money;
    }

    public void setMoney(double money) {
        this.money = money;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public static int getNumberOfShoppers() {
        return numberOfShoppers;
    }

    public static void setNumberOfShoppers(int numberOfShoppers) {
        Shopper.numberOfShoppers = numberOfShoppers;
    }


///
    public boolean isInArmy() {
        return inArmy;
    }

    public void setInArmy(boolean inArmy) {
        this.inArmy = inArmy;
    }

    public void setNumberInRank(int numberInRank) {
        this.numberInRank = numberInRank;
    }

    public boolean isxStay() {
        return xStay;
    }

    public void setxStay(boolean xStay) {
        this.xStay = xStay;
    }

    public boolean isyStay() {
        return yStay;
    }

    public void setyStay(boolean yStay) {
        this.yStay = yStay;
    }
//
    @Override
    public Shopper clone() throws CloneNotSupportedException {
        Shopper temp = (Shopper) super.clone();
        if (this.instrument !=null)
            temp.instrument = this.instrument.clone();
        temp.shadow = new Ellipse(75,20);
        temp.shadow.setFill(this.shadow.getFill());
        temp.shadow.setOpacity(this.shadow.getOpacity());
        temp.shadow.getTransforms().addAll(this.shadow.getTransforms());
        temp.shadow.setEffect(this.shadow.getEffect());
        temp.triangleAct = new Polygon();
        temp.triangleAct.getPoints().addAll(this.triangleAct.getPoints());
        temp.triangleAct.setFill(this.triangleAct.getFill());
        temp.shopperImage = new ImageView();
        temp.shopperImage.setPreserveRatio(this.shopperImage.isPreserveRatio());
        temp.shopperImage.setFitHeight(this.shopperImage.getFitHeight());
        temp.shopperImage.setEffect(this.shopperImage.getEffect());
        temp.shopperImage.setImage(this.shopperImage.getImage());
        temp.shopperText = new Label();
        temp.shopperText.setFont(this.shopperText.getFont());
        if (temp instanceof OrchestraConductor && this instanceof OrchestraConductor){
            ((OrchestraConductor) temp).setInstruments(new HashMap<>());
            ((OrchestraConductor) this).getInstruments().forEach((k,v)->{
                try {
                    ((OrchestraConductor) temp).getInstruments().put(k,v.clone());
                } catch (CloneNotSupportedException e) {
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setContentText("Помилка клонування в інструментах диригента");
                    alert.showAndWait();
                }
            });
        }
        if (temp.instrument != null) {
            temp.shopperPicture = new Group(temp.shopperImage, temp.shopperText, temp.shadow, temp.triangleAct, temp.instrument.getInstrumentImage());
        } else {
            temp.shopperPicture = new Group(temp.shopperImage, temp.shopperText, temp.shadow, temp.triangleAct);
        }
        return temp;
    }
    /*public static Comparator<Shopper> moneyComparator = new Comparator<Shopper>() {
        @Override
        public int compare(Shopper o1, Shopper o2) {
            return (int) (o1.money-o2.money);
        }
    };
    public static Comparator<Shopper> nameComparator = new Comparator<Shopper>() {
        @Override
        public int compare(Shopper o1, Shopper o2) {
            return o1.name.compareTo(o2.name);
        }
    };*/
    public void moveToArmy(){
        if (this.isActive) {
            if (Math.abs(this.xChord-Army.START_X*this.numberInRank)<5){
                //це для того, щоб не тряслись
                this.xStay = true;
            } else if (this.xChord<Army.START_X*this.numberInRank){
                this.xChord+=4;
            }else if (this.xChord>Army.START_X*this.numberInRank){
                this.xChord-=4;
            }
            if (Math.abs(this.yChord-100)<5){
                //це для того, щоб не тряслись
                this.yStay =true;
            } else if (this.yChord < 100) {
                this.yChord += 4;
            } else if (this.yChord > 100) {
                this.yChord -= 4;
            }
        } else {
            if (Math.abs(this.xChord-Army.START_X*this.numberInRank)<5){
                //це для того, щоб не тряслись
                this.xStay = true;

            } else if (this.xChord<Army.START_X*this.numberInRank){
                this.xChord+=4;
            }else if (this.xChord>Army.START_X*this.numberInRank){
                this.xChord-=4;
            }
            if (Math.abs(this.yChord-400)<5){
                //це для того, щоб не тряслись
                this.yStay =true;

            } else if (this.yChord < 400) {
                this.yChord += 4;
            } else if (this.yChord > 400) {
                this.yChord -= 4;
            }
        }
    }
}







