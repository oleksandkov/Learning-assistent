package sample;

import sample.micro.Heavy;
import sample.micro.Scout;
import sample.micro.StarShip;
import sample.secondMacro.Planet;

import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.*;

import java.util.ArrayList;
import java.util.HashMap;

public class Serialization {

    public static void serializeNow(File file) {
        XMLEncoder encoder;
        try {
            encoder = new XMLEncoder(new BufferedOutputStream(new FileOutputStream(file)));

            HashMap<String, Object> hashMap = new HashMap<>();
            hashMap.put("ships", Main.getWorld().getShips());
            hashMap.put("planets",Main.getWorld().getPlanets());
            encoder.writeObject(hashMap);
            encoder.close();
        } catch (FileNotFoundException e) {
            System.out.println("Помилка відкриття файлу");
        }
    }
    public static void deserializeNow(File file){
        XMLDecoder decoder;
        try {
            decoder=new XMLDecoder(new BufferedInputStream(new FileInputStream(file)));

            for (int i = 0; i<Main.getWorld().getShips().size(); i++){
                Scout ship = Main.getWorld().getShips().get(i--);
                Main.getWorld().deleteShip(ship);
            }
            for (int i = 0; i<Main.getWorld().getPlanets().size(); i++){
                Planet planet = Main.getWorld().getPlanets().get(i--);
                Main.getWorld().deletePlanet(planet);
            }
            HashMap<String, Object> hashMap = (HashMap<String, Object>)decoder.readObject();

            for (Planet planet: (ArrayList<Planet>)hashMap.get("planets")){
                Main.getWorld().addNewPlanet(planet);
            }
            for (Scout ship:(ArrayList<Scout>)hashMap.get("ships")){
                if (ship.getSide().equals("Green")){
                    System.out.println(ship.getSide());
                    Main.getWorld().addNewShip(ship, false);
                }
                if (ship.getSide().equals("Red")){
                    System.out.println("Ship blue");
                    switch (ship.getType()){
                        case "Scout":
                            Main.getWorld().addNewShip(new Scout(ship.getName().toString(),ship.getIsActive(),5,500,5,200,ship.getSide(),(int)ship.getChordX(),(int)ship.getChordY()),false);
                            break;
                        case "Heavy":
                            Main.getWorld().addNewShip(new Heavy(ship.getName().toString(),ship.getIsActive(),8,1000,8,200,ship.getSide(),(int)ship.getChordX(),(int)ship.getChordY()),false);
                            break;
                        case "StarShip":
                            Main.getWorld().addNewShip(new StarShip(ship.getName().toString(),ship.getIsActive(),10,2000,11,200,ship.getSide(),(int) ship.getChordX(),(int)ship.getChordY()),false);
                            break;
                    }
                }

            }
            decoder.close();
        } catch (FileNotFoundException e) {
            System.out.println("Помилка відкриття файлу");
        }
    }
}
