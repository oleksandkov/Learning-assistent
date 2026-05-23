package sample.secondMacro;

import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;

public class GreenPlanet extends Planet {

//getters

    public GreenPlanet(){
        numberOfPlanets++;

        this.chordX = 1000;
        this.chordY = 1000;

        this.planetType = "GreenPlanet";

        planetImage = new ImageView(new Image("images/greenPlanet.png"));
        planetImage.setPreserveRatio(true);
        planetImage.setFitHeight(350);

        this.planetName = new Text("Green Planet");
        this.planetName.setFont(new Font("Monaco", 20));
        this.planetName.setFill(Color.WHITE);

        this.activeZone = new Circle(300);
        this.activeZone.setFill(Color.LIGHTGREEN);
        this.activeZone.setOpacity(0.4);

        counterText = new Text("Всередині: ");
        counterText.setFont(new Font("Monaco", 20));
        counterText.setFill(Color.WHITE);

        this.planetGroup = new Group(planetImage,activeZone, planetName, counterText);
    }

    @Override
    public void setPlanetChord() {
        planetImage.setX(this.chordX);
        planetImage.setY(this.chordY);

        this.planetName.setX(this.chordX);
        this.planetName.setY(this.chordY - 40);

        this.activeZone.setLayoutX(this.chordX + 170);
        this.activeZone.setLayoutY(this.chordY + 170);

        counterText.setX(this.chordX);
        counterText.setY(this.chordY - 20);
    }
}
