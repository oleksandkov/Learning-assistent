import java.util.ArrayList;

public class Base {
    ArrayList<Unit> units;

    // Constructor
    public Base(ArrayList<Unit> units) {
        this.units = units;
    }
    public Base () {
        this.units = new ArrayList<>();
    }

    // Setters

    public void setUnits(ArrayList<Unit> units) {
        this.units = units;
    }

    // Getters

    public ArrayList<Unit> getUnits() {
        return this.units;
    }

}
