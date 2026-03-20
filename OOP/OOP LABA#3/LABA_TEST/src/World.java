import java.util.ArrayList;

public class World {
    private ArrayList<Unit> units;

    public static int objects = Unit.getNumObjects();


    // Constructors
    public World() {
        units = new ArrayList<>();
    }

    public World(ArrayList<Unit> units) {
        this.units = units;
    }

    // Getter
    public ArrayList<Unit> getUnits() {
        return units;
    }

    // Setter
    public void setUnits(ArrayList<Unit> units) {
        this.units = units;
    }


    // Print all units
    public void printUnits() {
        for (int i = 0; i < units.size(); i++) {
            System.out.println("Unit " + i + ":");
            System.out.println(units.get(i));
        }
    }

    // Add a unit at a specific position
    public void insertUnit(int index, Unit unit) {
        units.add(index, unit);
    }
}
