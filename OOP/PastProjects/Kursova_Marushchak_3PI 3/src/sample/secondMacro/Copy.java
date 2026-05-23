package sample.secondMacro;

public class Copy {
    private String name;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
    public Copy(String name){
        this.name = name;
    }

    @Override
    public Copy clone() throws CloneNotSupportedException {
        return (Copy) super.clone();
    }
}
