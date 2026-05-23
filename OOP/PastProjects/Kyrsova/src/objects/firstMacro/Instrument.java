package objects.firstMacro;

import javafx.scene.image.ImageView;

public abstract class Instrument implements Cloneable{
     protected double price;
     protected ImageView instrumentImage = null;
     protected String type = "Nothing";

     public abstract void update(double x, double y);

    public static Instrument getInstrument(String type){
        switch (type) {
            case "Guitar":
                return new Guitar();
            case "Drums":
                return new Drums();
            case "Bayan":
                return new Accordion();
            case "Piano":
                return new Piano();
            case "Trembita":
                return new Trembita();
            case "Violin":
                return new Violin();
            case "Nothing":
                return null;
        }
        return null;
    }


    public double getPrice() {
        return price;
    }

    public void setPrice(double price) {
        this.price = price;
    }

    public ImageView getInstrumentImage() {
        return instrumentImage;
    }

    public void setInstrumentImage(ImageView instrumentImage) {
        this.instrumentImage = instrumentImage;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    @Override
    public Instrument clone() throws CloneNotSupportedException {
        return (Instrument) super.clone();
    }

}
