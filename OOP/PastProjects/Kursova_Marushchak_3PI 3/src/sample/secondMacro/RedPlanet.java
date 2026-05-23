package sample.secondMacro;

import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;

public class RedPlanet extends Planet {

    public RedPlanet(){
        numberOfPlanets++;

        this.chordX = 4000;
        this.chordY = 2000;

        this.planetType = "RedPlanet";

        this.planetImage = new ImageView(new Image("images/redPlanet.png"));
        this.planetImage.setPreserveRatio(true);
        this.planetImage.setFitHeight(350);

        this.planetName = new Text("Red Planet");
        this.planetName.setFont(new Font("Monaco", 20));
        this.planetName.setFill(Color.WHITE);

        this.activeZone = new Circle(300);
        this.activeZone.setFill(Color.LIGHTCORAL);
        this.activeZone.setOpacity(0.4);

        counterText = new Text("Всередині: ");
        counterText.setFont(new Font("Monaco", 20));
        counterText.setFill(Color.WHITE);

        this.planetGroup = new Group(planetImage,activeZone, planetName, counterText);
    }

    @Override
    public void setPlanetChord() {
        this.planetImage.setX(this.chordX);
        this.planetImage.setY(this.chordY);

        this.planetName.setX(this.chordX);
        this.planetName.setY(this.chordY - 40);

        this.activeZone.setLayoutX(this.chordX + 170);
        this.activeZone.setLayoutY(this.chordY + 170);

        counterText.setX(this.chordX);
        counterText.setY(this.chordY - 20);
    }
}
