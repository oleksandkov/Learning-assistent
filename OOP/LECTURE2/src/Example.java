import java.util.HashMap;
import java.util.Map;

public class Example {
    private String name;
    private Integer kurs;
    private Double seredniBal;
    private Map<String, Double> zachotka;

    public static double stupendiumPorig = 4.0;

    public String getName() { //аксесор
        return name;
    }

    public void setName(String Name) { // мутатор, setter
        this.name = Name;
    }

    public Double getSeredniBal() { // getter
        return seredniBal;
    }

    public Example(String name, Integer kurs) {
        this.name = name;
        this.kurs = kurs;

        seredniBal = 0.0;
        zachotka =new HashMap<>();

        System.out.println("Constructor: Example(String name, Integer kurs) ");
        System.out.println("Object created");
        print();
    }

    public void print() {
        System.out.println("Student: " +name+" Kurs: " + kurs +" Serednibal: " + seredniBal + " Zachotka" + zachotka + "Stupendium Porig: " + stupendiumPorig);

    }

    public Example() {
        this("Nigger", 1);
    }
}
