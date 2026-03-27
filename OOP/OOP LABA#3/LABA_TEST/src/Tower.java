import java.util.ArrayList;

public class Tower {
    ArrayList<Unit> units;
    
    
    // Constructor
    public Tower(ArrayList<Unit> units) {
        this.units = units;
    }
    public Tower() {
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
