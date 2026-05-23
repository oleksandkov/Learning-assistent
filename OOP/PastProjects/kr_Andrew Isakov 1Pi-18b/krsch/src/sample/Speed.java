package sample;

public class Speed implements Cloneable{
    private double speed;
    Speed(){
       setSpeed(3);
    }

    public double getSpeed(){return speed;}

     public void setSpeed(double s) {
        if(s<1){System.out.println("slowwwww"); this.speed = 1;}
        else if(s>10) this.speed = 10;
        else this.speed = s;
    }
    protected Speed clone() throws CloneNotSupportedException
    {
        return (Speed) super.clone();
    }

    public int compareTo(Speed s) {
       if(this.speed > s.speed) return 1;
        if(this.speed < s.speed) return -1;
        else return 0;
    }

    public boolean equals(Speed o){
        if(this.speed == o.getSpeed()) return true;
        else return false;
    }
}
