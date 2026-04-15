package com.example.laba_main;
import java.util.ArrayList;
import java.util.Iterator;

public class World {
    private ArrayList<Unit> units;

    public static int objects = Unit.getNumObjects();

    public World() {
        units = new ArrayList<>();
    }

    public World(ArrayList<Unit> units) {
        this.units = units;
    }

    public ArrayList<Unit> getUnits() {
        return units;
    }

    public void setUnits(ArrayList<Unit> units) {
        this.units = units;
    }

    public void printUnits() {
        if (units == null || units.isEmpty()) {
            System.out.println("No units in this object.");
            return;
        }
        for (int i = 0; i < units.size(); i++) {
            System.out.println("Unit " + i + ":");
            System.out.println(units.get(i));
        }
    }

    public void insertUnit(int index, Unit unit) {
        if (units == null) {
            units = new ArrayList<>();
        }
        units.add(index, unit);
    }



    public void update() {
        if (units != null) {
            Iterator<Unit> it = units.iterator();
            while (it.hasNext()) {
                Unit u = it.next();
                if (u == null || Boolean.TRUE.equals(u.getDead()) || (u.getHealth() != null && u.getHealth() <= 0)) {
                    it.remove();
                    Unit.removeUnit();
                }
            }
        }
    }

    public static void updateAll(World world, World base, World tower) {
        if (world != null) world.update();
        if (base != null) base.update();
        if (tower != null) tower.update();
    }


}

