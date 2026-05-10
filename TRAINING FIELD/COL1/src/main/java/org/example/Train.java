package org.example;

import java.util.HashMap;

public class Train {
    private String title;
    private HashMap<String, Integer> passengers;

    public Train(String title, HashMap<String, Integer> passengers) {
        this.title = title;
        this.passengers = passengers;
    }
    // Deep copy constructor
    public Train(Train other) {
        this.title = other.title;
        // deep copy of the map
        this.passengers = new HashMap<>();
        for (String k : other.passengers.keySet()) {
            this.passengers.put(k, other.passengers.get(k));
        }
    }
    public Train() {
        this.title = "Train N";
        this.passengers = new HashMap<>();
    }

    public String getTitle() {
        return title;
    }

    public HashMap<String, Integer> getPassengers() {
        return passengers;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public void setPassengers(HashMap<String, Integer> passengers) {
        this.passengers = passengers;
    }


    // overload: add by name and place
    public void add(String name, int place) {
        this.passengers.put(name, place);
    }
  
    // remove by name
    public void remove(String name) {
        this.passengers.remove(name);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Train other = (Train) obj;
        if (title == null) {
            if (other.title != null) return false;
        } else if (!title.equals(other.title)) return false;
       return this.passengers.equals(other.passengers);
    }

    @Override
    public int hashCode() {
        int result = title != null ? title.hashCode() : 0;
        result = 31 * result + (passengers != null ? passengers.hashCode() : 0);
        return result;
    }
}
