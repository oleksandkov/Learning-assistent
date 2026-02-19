import static java.lang.Math.max;
import java.util.Objects;
import java.util.Scanner;

public class Unit {
    private Integer health;
    private Boolean isSpawned;
    private String Team;
    private Integer damage; 
    private Boolean isDead;
    
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
    public Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead) {
        this.health = health;
        this.isSpawned = isSpawned;
        Team = team;
        this.damage = damage;
        this.isDead = isDead;
        System.out.println("Constructor: Unit(Integer health, Boolean isSpawned, String team, Integer damage, Boolean isDead)");
        System.out.println("Object created. Total objects: " + numObjects);
    }

    public  Unit() {
        this(100, false, "ally", 5, true);
    }

    
    public static int getNumObjects() {
        return numObjects;
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
    // boolen equals()
    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return Objects.equals(getHealth(), unit.getHealth()) && Objects.equals(isSpawned, unit.isSpawned) && Objects.equals(getTeam(), unit.getTeam()) && Objects.equals(getDamage(), unit.getDamage()) && Objects.equals(isDead, unit.isDead);
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
                '}';
    }
    // Additional functions
    public void addHealth() {
       this.health += 10;
    }
    public  void addDamage() {
        this.damage += 5;
    }
    public void changeTeam() {
        this.Team = "enemy";
    }
    public void takeDamage() {
        this.health -= 10;
        if (this.health <= 10) {
            System.out.println("Health is too low to take any damage");
        }
    }
    public void showTheStrongest(Unit unit) {
        int maxhealth = max(this.health, unit.health);
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
        System.out.println("Set team: ");
        String team;
        team = scanner.nextLine();
        this.setTeam(team);
        scanner.nextLine();
        System.out.println("Set damage: ");
        int damage;
        damage = scanner.nextInt();
        this.setDamage(damage);
        scanner.nextLine();
    }
    // Print
    public void print() {
        System.out.println("THE HEALTH: " + health);
        System.out.println("IF UNIT IS SPAWNED: " + isSpawned);
        System.out.println("THE DAMAGE OF THE UNIT: " + damage);
        System.out.println("THE TEAM " + Team);
        System.out.println("IF THE ENEMY ALIVE: " + isDead);

    }

}
