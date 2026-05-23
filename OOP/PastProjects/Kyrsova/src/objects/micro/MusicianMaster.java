package objects.micro;


import army.Army;
import javafx.animation.Animation;
import javafx.scene.Group;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.effect.DropShadow;
import javafx.scene.effect.GaussianBlur;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.media.AudioClip;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.scene.paint.Color;
import javafx.scene.shape.Ellipse;
import javafx.scene.shape.Polygon;
import javafx.scene.text.Font;
import javafx.scene.transform.Rotate;
import javafx.util.Duration;
import objects.firstMacro.Instrument;
import sample.Sprite;
import sample.windows.preferencesWindow.Preferences;

import java.nio.file.Paths;
import java.util.Optional;

public class MusicianMaster extends Shopper {

    public MusicianMaster(Instrument instrument, boolean isActive, String name, double money , boolean isMan){
        super(instrument, isActive,  name,  money ,  isMan);
        this.type = "Master";
        this.playVariable = 1;

        if (isMan){
            this.shopperImage  = new ImageView(new Image("assets/master.png"));
        } else this.shopperImage  = new ImageView(new Image("assets/girlMaster.png"));

        this.shopperImage.setPreserveRatio(true);
        this.shopperImage.setFitHeight(190);
        this.shopperImage.setEffect(new DropShadow(30,0,0, Color.GRAY));

        this.instrument = instrument;

        this.shopperText = new Label(this.name + ", money: "+ this.money);
        this.shopperText.setFont(new Font("Segoe UI Black Italic", 13));

        this.shadow = new Ellipse(95, 20);
        this.shadow.setFill(Color.BLACK);
        this.shadow.setOpacity(0.9);
        this.shadow.getTransforms().add(new Rotate(20));
        this.shadow.setEffect(new GaussianBlur(40));

        this.triangleAct = new Polygon();
        triangleAct.getPoints().addAll(0.0, 0.0,
                30.0, 0.0,
                15.0, 25.0);
        this.triangleAct.setFill(Color.BLACK);

        if (this.instrument != null){
            this.shopperPicture = new Group(shopperImage, shopperText, shadow, triangleAct, this.instrument.getInstrumentImage());
        } else {
            this.shopperPicture = new Group(shopperImage, shopperText, shadow, triangleAct);
        }
    }
    public MusicianMaster(boolean isActive, String name, double money, boolean isMan) {
        this(null, isActive, name, money,isMan);
    }
    public MusicianMaster( Instrument instrument,boolean isActive, String name) {
        this(instrument, isActive, name, 0,true);
    }
    public MusicianMaster( boolean isActive, String name) {
        this(null, isActive, name, 0, true);
    }
    public MusicianMaster(boolean isActive, String name, double money){
        this(null, isActive, name, money, true);
    }
    public MusicianMaster(String name){
        this(null, true, name, 0,true);
    }
    public MusicianMaster(){
        this(null, false,null,0,true);
    }

    @Override
    public void setShopperInChords() {

        this.shopperImage.setX(this.xChord);
        this.shopperImage.setY(this.yChord);

        this.shopperText.setLayoutX(this.xChord-15);
        this.shopperText.setLayoutY(this.yChord -17);
        this.shopperText.setText(this.name + ", money: "+ this.money);


        this.shadow.setLayoutX(this.xChord -10);
        this.shadow.setLayoutY(this.yChord + 170);

        this.triangleAct.setLayoutX(this.xChord+30);
        this.triangleAct.setLayoutY(this.yChord-40);

        if (this.isActive) {
            this.triangleAct.setOpacity(1);
        } else {
            this.triangleAct.setOpacity(0);
        }
        if (this.instrument != null) {
            this.instrument.update(this.xChord, this.yChord);
        }

    }

    @Override
    public Shopper education(boolean first, boolean second, double allPrice) {
        if (second && !first && this.money >= allPrice)
            if (this.isMan)
                return new OrchestraConductor(Instrument.getInstrument(this.instrument.getType()), true, this.name, this.money-allPrice);
            else
            {
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setContentText("Жінки не можуть бути дирижорами");
                alert.showAndWait();
            }
            else if (second && !first && this.money <= allPrice)
        {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("Недостатньо коштів");
            alert.showAndWait();
        } else{
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("Ви уже пройшли цю програму!");
            alert.showAndWait();
        }
        return this;
    }

    @Override
    public void sellAnInstrument() {
        if (this.instrument != null){
            Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
            alert.setContentText("Ви дійсно хочете продати поточний інструмент?");
            Optional<ButtonType> option = alert.showAndWait();
            if (option.get() == ButtonType.OK){
                this.shopperPicture.getChildren().remove(this.instrument.getInstrumentImage());
                this.money+= this.instrument.getPrice()* 0.5;
                this.instrument = null;
            }
        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("У вас немає що продавати");
            alert.showAndWait();
        }
    }

    @Override
    public void playAnInstrument() {

        this.shopperImage.setOpacity(0);
        ImageView masterPlaySprite;
        if (this.isMan) {
             masterPlaySprite = new ImageView(new Image("assets/masterPlaySprite.png"));
        } else {
             masterPlaySprite = new ImageView(new Image("assets/girlMasterPlaySprite.png"));
        }
        masterPlaySprite.setX(this.shopperImage.getX());
        masterPlaySprite.setY(this.shopperImage.getY());
        masterPlaySprite.setPreserveRatio(true);
        masterPlaySprite.setFitHeight(this.shopperImage.getFitHeight());


        this.getShopperPicture().getChildren().add(4 ,masterPlaySprite);
        this.setStay(true);

        Animation playAnimation = new Sprite(masterPlaySprite , Duration.millis(2000),4,4,431,683);
        playAnimation.setCycleCount(10);//як довго буде грати

        String musicPath = "";
        switch (this.instrument.getType()){
            case "Guitar":
                musicPath = "src/assets/music/guitarGood.mp3";
                break;
            case "Bayan":
                musicPath = "src/assets/music/bayanGood.mp3";
                break;
            case "Drums":
                musicPath = "src/assets/music/drumsGood.mp3";
                break;
            case "Piano":
                musicPath = "src/assets/music/pianoGood.mp3";
                break;
            case "Trembita":
                musicPath = "src/assets/music/trembitaGood.mp3";
                break;
            case "Violin":
                musicPath = "src/assets/music/despasito.mp3";
                break;
        }
        Media hit = new Media(Paths.get(musicPath).toUri().toString());
        MediaPlayer mediaPlayer = new MediaPlayer(hit);
        mediaPlayer.setVolume(Preferences.getVOLUME());
        playAnimation.play();
        mediaPlayer.play();



        playAnimation.setOnFinished(event -> {
            this.shopperImage.setOpacity(1);
            this.getShopperPicture().getChildren().remove(masterPlaySprite);
            this.setStay(false);
            mediaPlayer.stop();
        });

    }

    @Override
    public void setMan(boolean man) {
        isMan = man;
        if (man)
            this.shopperImage.setImage(new Image("assets/master.png"));
        else this.shopperImage.setImage(new Image("assets/girlMaster.png"));
    }

    @Override
    public void moveToArmy(){

        if (this.isActive) {
            if (Math.abs(this.xChord-Army.START_X*this.numberInRank)<5){
                //це для того, щоб не тряслись
                this.xStay = true;

            } else if (this.xChord< Army.START_X*this.numberInRank){
                this.xChord+=4;
            }else if (this.xChord>Army.START_X*this.numberInRank){
                this.xChord-=4;
            }
            if (Math.abs(this.yChord-700)<5){
                //це для того, щоб не тряслись
                this.yStay =true;

            } else if (this.yChord < 700) {
                this.yChord += 4;
            } else if (this.yChord > 700) {
                this.yChord -= 4;
            }
        } else {
            if (Math.abs(this.xChord-Army.START_X*this.numberInRank)<5){
                //це для того, щоб не тряслись
                this.xStay = true;

            } else
            if (this.xChord< Army.START_X*this.numberInRank){
                this.xChord+=4;
            }else if (this.xChord>Army.START_X*this.numberInRank){
                this.xChord-=4;
            }
            if (Math.abs(this.yChord-1000)<5){
                //це для того, щоб не тряслись
                this.yStay =true;

            } else if (this.yChord < 1000) {
                this.yChord += 4;
            } else if (this.yChord > 1000) {
                this.yChord -= 4;
            }
        }
    }

}

