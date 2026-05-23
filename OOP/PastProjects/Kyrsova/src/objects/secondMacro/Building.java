package objects.secondMacro;


import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.shape.Ellipse;
import objects.micro.Shopper;



public abstract class Building {
    protected double xChord;
    protected double yChord;

    protected ImageView buildingImage;
    protected Label buildingText;
    protected Ellipse shadow;

    protected Group buildingPicture;
    protected static int numberOfBuildings = 0;

    protected String buildingType = "Nothing";

    public Group getBuildingPicture() {
        return this.buildingPicture;
    }

    public void setXChord(double xChord) {
        this.xChord = xChord;
    }

    public void setYChord(double yChord) {
        this.yChord = yChord;
    }

    public double getXChord() {
        return xChord;
    }

    public double getYChord() {
        return yChord;
    }

    public static int getNumberOfBuildings() {
        return numberOfBuildings;
    }

    public static void setNumberOfBuildings(int numberOfBuildings) {
        Building.numberOfBuildings = numberOfBuildings;
    }

    public String getType() {
        return this.buildingType;
    }

    public ImageView getBuildingImage() {
        return buildingImage;
    }

    public abstract void setBuildingInChords();
    public abstract boolean interact(Shopper shopper, boolean isShiftDown);
}
