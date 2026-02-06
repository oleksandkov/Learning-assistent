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
        if (getAverPrice() > x.getAverPrice()) System.out.println(getName() + "is better than " + x.getName());
        else System.out.println(x.getName() + "is better than" + getName());
    }


}


public class Main2 {
    public static void main(String[] args) {
        Shop x;
        Shop y;
        Shop z;
        System.out.println("Here some shops:");
        x = new Shop("ATB", "SuperMarket", "st. Kovmonavtiv", 3, "2025", 192.5, true);
        y = new Shop("Comfy", "TechnoShop", "trc. Megamoll", 12, "2023", 600.5, true);
        z = new Shop("Bazar", "Market", "trc. Megamoll", 2, "1999", 24.67, false);

        x.showstat();
        y.showstat();
        z.showstat();

        System.out.println("My favorite is " + x.getName());

        System.out.println("COMPARISON:");
        x.compareAverPrice(y);
        System.out.println("The setters usage:");
        x.setType("SUPPERPUPPERMARKET");
        System.out.println("New Type of " + x.getName() + " is " + x.getType());

        System.out.println("The Creation of hte new shop: ");
        String Type;
        String Name;
        String Location;
        Integer NumEmpl;
        String Date;
        double price;
        boolean boolStatus;

        Scanner scanner = new Scanner(System.in);
        System.out.println("Set the name of shop:");
        Name = scanner.nextLine();
        
        System.out.println("Set the Type of shop:");
        Type = scanner.nextLine();
        
        System.out.println("Set the location: ");
        Location = scanner.nextLine();
        
        System.out.println("Set the Number of employers in a shop: ");
        NumEmpl = scanner.nextInt();
        
        System.out.println("Set the date of the opening: ");
        scanner.nextLine();
        Date = scanner.nextLine();
        
        System.out.println("Set the average price in a shop: ");
        price = scanner.nextDouble();
        
        System.out.println("If it still working? (true/false): ");
        boolStatus = scanner.nextBoolean();


        Shop own = new Shop(Name, Type, Location, NumEmpl, Date, price, boolStatus);

        System.out.println("Here your new shop:");
        own.showstat();

        scanner.close();
    }
}
