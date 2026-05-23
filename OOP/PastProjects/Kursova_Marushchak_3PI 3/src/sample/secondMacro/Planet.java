package sample.secondMacro;

import javafx.scene.Group;
import javafx.scene.image.ImageView;
import javafx.scene.shape.Circle;
import javafx.scene.text.Text;

public abstract class Planet {
    protected static int numberOfPlanets = 0;

    protected double chordX;
    protected double chordY;

    protected  ImageView planetImage;
    protected Group planetGroup;

    protected Text planetName;
    protected String planetType;
    protected Circle activeZone;

    protected Text counterText;


    //getters
    public ImageView getPlanetImage() {
        return this.planetImage;
    }
    public String getPlanetType() {
        return planetType;
    }
    public double getChordY() {
        return chordY;
    }
    public double getChordX() {
        return chordX;
    }
    public Group getPlanetGroup() {
        return planetGroup;
    }
    public Circle getActiveZone() {
        return activeZone;
    }
    public static int getNumberOfPlanets() {
        return numberOfPlanets;
    }

    //setters
    public abstract void setPlanetChord();
    public void setCounterGreenText(int counter) {
        this.counterText.setText("Всередині: " + counter);
    }
    public void setCounterRedText(int counter) {
        this.counterText.setText("Всередині: " + counter);
    }
    public void setCounterMoonText(int counter) {
        this.counterText.setText("Всередині: " + counter);
    }
    public static void setNumberOfPlanets(int numberOfPlanets) {
        Planet.numberOfPlanets = numberOfPlanets;
    }
}

