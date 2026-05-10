package org.example;

public class Passenger {
    private String name;
    private int place;

    public Passenger(String name, int place) {
        this.name = name;
        this.place = place;
    }
    public Passenger() {
        this.name = "BaRAK";
        this.place = 1;
    }

    public String getName() {
        return name;
    }

    public int getPlace() {
        return place;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setPlace(int place) {
        this.place = place;
    }
}
