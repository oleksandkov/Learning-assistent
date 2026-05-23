package sample.micro;

import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import sample.Main;

public class Heavy extends Scout {

    public Heavy(String name, boolean isActive, int speed, int health, int damage, int experience, String side, int chordX, int chordY){
        this(name, isActive, speed, health, damage, experience, side);
        this.chordX = chordX;
        this.chordY = chordY;
    }

    public Heavy(){
        numberOfShips++;
        this.type = "Heavy";
        this.side = "Green";

        this.isActive = false;
        this.speed = 8;
        this.health = 1000;
        this.damage = 8;
        this.experience = 200;
        this.deltaHealth = 0.1;

        try{
            this.shipImage = new ImageView(new Image("images/Heavy.png"));
        }catch (Exception e){
            System.out.println("Error!");
        }

        this.shipImage.setFitHeight(100);
        this.shipImage.setPreserveRatio(true);

        this.name= new Text(Main.getNamesArray()[Main.random.nextInt(Main.getNamesArray().length)]);
        this.name.setFont(new Font("Muna", 20));
        this.name.setFill(Color.WHITE);

        this.rectActive = new Rectangle(120,100);
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
        } );

    }

    public Heavy(String name, boolean isActive, int speed, int health, int damage, int experience, String side) {
        super(name, isActive, speed, health, damage, experience, side);
        numberOfShips++;
        this.type = "Heavy";
        this.side = side;
        this.isActive = isActive;
        this.speed = speed;
        this.health = health;
        this.damage = damage;
        this.experience = experience;
        this.deltaHealth = 0.1;

        if ("Green".equals(this.side)) {
            this.nativePlanetX = 1000;
            this.nativePlanetY = 1000;
        } else {
            this.nativePlanetX = 4000;
            this.nativePlanetY = 2000;
        }

        try{
            if (this.side.equals("Green")){
                this.shipImage = new ImageView(new Image("images/heavy.png"));
            }else{
                this.shipImage = new ImageView(new Image("images/heavySideTwo.png"));
            }

        }catch (Exception e){
            System.out.println("Error!");
        }

        this.shipImage.setFitHeight(100);
        this.shipImage.setPreserveRatio(true);

        this.name = new Text(name);
        this.name.setFont(new Font("Muna", 20));
        this.name.setFill(Color.WHITE);

        this.rectActive = new Rectangle(120,100);
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
        } );
    }


    public void sayHello(){
        System.out.print(this.type + "   ");
        System.out.print(this.name.getText() + ":");
        System.out.println(" Hello");
    }
}
