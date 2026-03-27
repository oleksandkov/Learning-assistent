import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Objects;
import java.util.Scanner;

public class Unit implements Cloneable{
    private Integer health;
    private Boolean isSpawned;
    private String Team;
    private Integer damage; 
    private Boolean isDead;
    private ArrayList<String> inventor;
    
    private static int numObjects = 0;
    private static int objectedKilled = 0;
    
    // Static Block
    static {
        System.out.println("STATIC BLOCK IS RUNT");
        System.out.println("Unit class loaded. Objects count: " + numObjects);
    }
    
    // Non-static initialization block
    {
        numObjects++;  
        System.out.println("INIT BLOCK IS RUNT");
        System.out.println("Object #" + numObjects + " is being initialized");
    }
    // Constructors
    public Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this.health = health;
        this.isSpawned = isSpawned;
        Team = team;
        this.damage = damage;
        this.isDead = isDead;
        this.inventor = inventor;
        System.out.println("Constructor: Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead, ArrayList<String> inventor)");
        System.out.println("Object created. Total objects: " + numObjects);
    }

    public  Unit() {
        this(100, false, "ally", 5, true, new ArrayList<>(Arrays.asList("sword")));
    }

    
    public static int getNumObjects() {
        return numObjects;
    }

    public static void setNumObjects(int num) {
        numObjects = num;
    }

    // Getters
    public Integer getHealth() {
        return health;
    }

    public Boolean getSpawned() {
        return isSpawned;
    }

    public String getTeam() {
        return Team;
    }

    public Integer getDamage() {
        return damage;
    }

    public Boolean getDead() {
        return isDead;
    }

    public ArrayList<String> getInventor() {
        return inventor;
    }

    public static int getnumObjects() {
        return numObjects;
    }
    // Nested classes for comparators
    public static class HealthComparator implements Comparator<Unit> {
    @Override
    public int compare(Unit a, Unit b) {
        return Integer.compare(a.getHealth(), b.getHealth());
    }
    }

    public static class TeamComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return a.getTeam().compareTo(b.getTeam());
        }
    }

    // CompareTo4.7
    
        public int compareTo47(Unit x) {
        int cmp = Integer.compare(this.health, x.health);
        if (cmp != 0) return cmp;
        cmp = this.Team.compareTo(x.Team);
        if (cmp != 0) return cmp;
        cmp = this.damage.compareTo(x.damage);
        if (cmp != 0) return cmp;
        cmp = this.isSpawned.compareTo(x.isSpawned);
        if (cmp != 0) return cmp;
        cmp = this.isDead.compareTo(x.isDead);
        if (cmp != 0) return cmp;
        cmp = this.health.compareTo(x.health);
        if (cmp != 0) return cmp;
        ArrayList<String> sortedInventor1 = new ArrayList<>(this.inventor);
        ArrayList<String> sortedInventor2 = new ArrayList<>(x.inventor);
        sortedInventor1.sort(String::compareTo);
        sortedInventor2.sort(String::compareTo);
        cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
        if (cmp != 0) return cmp;
        return 0;
        }




    // Setters


    public void setHealth(Integer health) {
        this.health = health;
    }

    public void setSpawned(Boolean spawned) {
        isSpawned = spawned;
    }

    public void setTeam(String team) {
        Team = team;
    }

    public void setDamage(Integer damage) {
        this.damage = damage;
    }

    public void setDead(Boolean dead) {
        isDead = dead;
    }

    public void setInventor(ArrayList<String> inventor) {
        this.inventor = inventor;
    }

    // boolen equals()

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return Objects.equals(health, unit.health) && Objects.equals(isSpawned, unit.isSpawned) && Objects.equals(Team, unit.Team) && Objects.equals(damage, unit.damage) && Objects.equals(isDead, unit.isDead) && Objects.equals(inventor, unit.inventor);
    }

    @Override
    public int hashCode() {
        return Objects.hash(health, isSpawned, Team, damage, isDead, inventor);
    }


    // String toString()

    @Override
    public String toString() {
        return "Unit{" +
                "health=" + health +
                ", isSpawned=" + isSpawned +
                ", Team='" + Team + '\'' +
                ", damage=" + damage +
                ", isDead=" + isDead +
                ", inventor=" + inventor +
                "}\n";
    }


    // clone()


    @Override
    protected Unit clone() throws CloneNotSupportedException {

        Unit clonedUnit = (Unit) super.clone();
        ArrayList<String> clonedInventor = (ArrayList<String>) clonedUnit.inventor.clone();

        clonedUnit.setInventor(clonedInventor);
        return clonedUnit;
    }

    // Additional functions
    public static void removeUnit() {
        numObjects--;
        System.out.println("Object removed. Total objects: " + numObjects);
    }
    public void addHealth() {
       this.health += 10;
    }
    public  void addDamage() {
        this.damage += 5;
    }
    public void changeTeam() {
        if (this.Team.equals("ally")) {
            this.Team = "enemy";
        } else {
            this.Team = "ally";
        }
    }
    public boolean isAlly() {
        return "ally".equals(this.Team);
    }
    public boolean isAlly(Unit x) {
        return this.Team != null && this.Team.equals(x.Team);
    }
    public void takeDamage() {
        this.health -= 10;
        if (this.health <= 10) {
            System.out.println("Health is too low to take any damage");
        }
    }
    public void showTheStrongest(Unit unit) {
        Unit maxhealth = (this.health > unit.health) ? this : unit;
        System.out.println("The strongest unit have " + maxhealth);
    }
    public void runSetters() {
        System.out.println("Set dead state: ");
        Scanner scanner = new Scanner(System.in);
        boolean isdead;
        isdead = scanner.nextBoolean();
        this.setDead(isdead);
        scanner.nextLine();
        System.out.println("Set health bar: ");
        int numHealth;
        numHealth = scanner.nextInt();
        this.setHealth(numHealth);
        scanner.nextLine();
        boolean spawnState;
        System.out.println("Set spawn state: ");
        spawnState = scanner.nextBoolean();
        this.setSpawned(spawnState);
        scanner.nextLine();
        System.out.println("Set team: ");
        String team;
        team = scanner.nextLine().trim();
        this.setTeam(team);
        System.out.println("Set damage: ");
        int damage;
        damage = scanner.nextInt();
        this.setDamage(damage);
        scanner.nextLine();
        System.out.println("Set inventor: ");
        String inventoryInput = scanner.nextLine().trim();
        if (inventoryInput.isEmpty()) {
            this.setInventor(new ArrayList<>());
        } else {
            this.setInventor(new ArrayList<>(Arrays.asList(inventoryInput.split("\\\\s+"))));
        }
        scanner.nextLine();
    }
    // Print
    public void print() {
        System.out.println("THE HEALTH: " + health);
        System.out.println("IF UNIT IS SPAWNED: " + isSpawned);
        System.out.println("THE DAMAGE OF THE UNIT: " + damage);
        System.out.println("THE TEAM " + Team);
        System.out.println("IF THE ENEMY ALIVE: " + isDead);
        System.out.println("THE INVENTORY: " + inventor);

    }

}
