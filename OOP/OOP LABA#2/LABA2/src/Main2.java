import javax.xml.crypto.Data;
import java.util.Scanner;

class Shop {
    private String Name;
    private String Type;
    private String Location;
    private Integer NumEmpl;
    private String DateOfOpen;
    private double AverPrice;
    private boolean StillWorking;

    // Constructor
    public Shop(String Name, String Type, String Location, int NumEmpl, String DatOfOpen, double AverPrice, boolean StillWorking) {
        this.Name = Name;
        this.Type = Type;
        this.Location = Location;
        this.NumEmpl = NumEmpl;
        this.DateOfOpen = DatOfOpen;
        this.AverPrice = AverPrice;
        this.StillWorking = StillWorking;
    }

    // Getters

    public String getName() {
        return Name;
    }

    public String getType() {
        return Type;
    }

    public String getLocation() {
        return Location;
    }

    public Integer getNumEmpl() {
        return NumEmpl;
    }

    public String getDateOfOpen() {
        return DateOfOpen;
    }

    public double getAverPrice() {
        return AverPrice;
    }

    public boolean getStillWorking() {
        return  StillWorking;
    }

    // Setters

    public void setName(String Name) {
        this.Name = Name;
    }

    public void  setType(String Type) {
        this.Type = Type;
    }

    public void setLocation(String Location) {
        this.Location = Location;
    }

    public void setNumEmpl(Integer NumEmpl) {
        this.NumEmpl = NumEmpl;
    }

    public void setDateOfOpen(String DateOfOpen) {
        this.DateOfOpen = DateOfOpen;
    }

    public void setAverPrice(double AverPrice) {
        this.AverPrice = AverPrice;
    }

    public  void setStillWorking(boolean StillWorking) {
        this.StillWorking = StillWorking;
    }

    // Other

    public void showstat() {

        System.out.println("The Name of the shop " + Name);
        System.out.println("The type of the shop " + Type);
        System.out.println("The location of the shop " + Location);
        System.out.println("The number of employers " + NumEmpl);
        System.out.println("The date when the shop was opened " + DateOfOpen);
        System.out.println("The average price across the shop " + AverPrice);
        System.out.println("IF the shop still exists " + StillWorking);
    }

    public void ifStillWorking() {
        if(getStillWorking() == true)  System.out.println("The shop is working");
        else System.out.println("The shop is closed");
    }

    public void compareAverPrice(Shop x) {

    }


}


public class Main2 {
    
}
