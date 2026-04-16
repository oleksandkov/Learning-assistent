package org.example.lab4;

import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

enum StudentStatus{
    BORZHNIK, ZVICHAJNIJ, STIPENDIAT, STIPENDIAT_KABMINA
}

public class Student implements Cloneable, Comparable<Student> {
    private String name;
    private int kurs;  // демонструє Comparable<Student>
    private double serednijBal;
    private Map<String, Double> zachotka = new HashMap<String, Double>();  //Таблиця(Дисципліна, Оцінка)
//-------------------------------------------------------
    private Label labelName;
    private double health;
    private static double healthMax=100;
    private Line life;
    private ImageView istudent;
    private double x, y;

    private double aimx;
    private double aimy;

    private boolean isActive;

    private Rectangle rectActive;

    //Якщо по якійсь дисципліні оцінка менше цього порогу, то переведення на наступний курс не можливе
    private static double akademichnijPorig;

    private static double kabminStipendiaPorig;

    private static double stipendiaPorig;
    static {
        //Ініціалізуються статичні параметри
        System.out.println("Статичний блок ініціалізації Student");
        setStipendiaPorig(4.0);
        kabminStipendiaPorig=4.5;
        akademichnijPorig=3.0;
    }

    public static double getStipendiaPorig() {
        return stipendiaPorig;
    }

    public static void setStipendiaPorig(double stipendiaPorig) {
        Student.stipendiaPorig = stipendiaPorig;

    }

    public StudentStatus getStatus(){
        boolean stipendiaflag=true;
        boolean kabminStipendiaflag=true;

        for(Map.Entry<String, Double> item: zachotka.entrySet() ) {
            if( item.getValue()<akademichnijPorig )return StudentStatus.BORZHNIK;

            if( item.getValue()<stipendiaPorig)stipendiaflag= false;
            if( item.getValue()<kabminStipendiaPorig)kabminStipendiaflag= false;

        }

        if( stipendiaflag==false )return StudentStatus.ZVICHAJNIJ ;

        if( kabminStipendiaflag==false) return StudentStatus.STIPENDIAT;

        return StudentStatus.STIPENDIAT_KABMINA;

    }

    //Назва говорить сама за себе
    public boolean perevodNaNastupnijKurs(){
        for(Map.Entry<String, Double> item: zachotka.entrySet() ) {
            if( item.getValue()<akademichnijPorig )return false;
        }

        ++kurs;
        return true;
    }



    public boolean hasStipendia(){
        if(serednijBal<stipendiaPorig)return false;

        return true;
    }

    //Копіювальний конструктор
    public Student( Student arg ){
        //arg -> this
        this.name= arg.name;
        this.kurs= arg.kurs;

        //this.zachotka = arg.zachotka; //Поверхневе копіювання (shallow copy)
        for(Map.Entry<String, Double> item: arg.zachotka.entrySet() ) {
            this.zachotka.put(item.getKey(), item.getValue());
        }

        updateSerednijBal();
    }

    @Override
    protected Object clone() throws CloneNotSupportedException {
        //return
        Student tmp = (Student)super.clone();

        tmp.zachotka= new HashMap<>();

        //this.zachotka -> tmp.zachotka; //Поверхневе копіювання (shallow copy)

        //Глибинне копіювання
        for(Map.Entry<String, Double> item: this.zachotka.entrySet() ) {
            tmp.zachotka.put(item.getKey(), item.getValue());
        }

        tmp.updateSerednijBal();

        return tmp;
    }

    public void clearAim()
    {
        aimx = aimy = -1000.0;
    }

    public boolean isActive(){
        return isActive;
    }

    public static double imageDeltaX(){
        return 0.0;
    }
    public static double imageDeltaY(){
        return 15+7;
    }

    //Конструктор з аргументами - ініціалізує параметри об'єкта, що створється
    public Student(String n, int kurs, double h, double _x, double _y ){
        name = n;
        this.kurs = kurs;
        serednijBal=0.0;

        x=_x;
        y=_y;

        clearAim();

        labelName=new Label(name);
        labelName.setLayoutX(x);
        labelName.setLayoutY(y);

        health=h;
        life=new Line(x,y+15, x+health,y+15);
        life.setStrokeWidth(5);
        life.setStroke(Color.LIGHTGREEN);

        istudent= new ImageView(HelloApplication.imgBachelor );
        istudent.setX(x+imageDeltaX());
        istudent.setY(y+imageDeltaY());

        isActive=false;

        rectActive= new Rectangle(x-5,y-5,105,105+imageDeltaY());
        rectActive.setFill(Color.TRANSPARENT);
        rectActive.setStrokeWidth(3);
        rectActive.setStroke(Color.YELLOW);

        //System.out.println("Викликано конструктор Student(String n, int kurs). Створено "+toString() );
    }
    public void resurrect(){
        HelloApplication.group.getChildren().addAll(labelName, life, istudent);

        if(isActive)HelloApplication.group.getChildren().add(rectActive);
    }

    public void setCoordinates(){
        labelName.setLayoutX(x);
        labelName.setLayoutY(y);


        life.setStartX(x);
        life.setStartY(y+15);
        life.setEndX(x+health);
        life.setEndY(y+15);

        //        World.group.getChildren().remove(life);
//        life=new Line(x,y+15, x+health,y+15);
//        life.setStrokeWidth(5);
//        life.setStroke(Color.LIGHTGREEN);
//        World.group.getChildren().add(life);

        istudent.setX(x+imageDeltaX());
        istudent.setY(y+imageDeltaY());

        rectActive.setX(x-5);
        rectActive.setY(y-5);

    }


    public void move( double dx, double dy ){

        x=x+dx;
        y=y+dy;
        setCoordinates();
    }

    public void harakiri(){
        HelloApplication.group.getChildren().removeAll(labelName, life, istudent);

       if(isActive)HelloApplication.group.getChildren().remove(rectActive);

    }

    public boolean flipActivation(){
        if(isActive){
            HelloApplication.group.getChildren().remove(rectActive);
        }
        else{
            HelloApplication.group.getChildren().add(rectActive);
        }
        isActive = !isActive;

        return isActive;
    }


    public boolean tryActivate( double mx, double my ){
        if(istudent.boundsInParentProperty().get().contains(mx,my)){
            flipActivation();
            return true;
        }
        return false;
    }
    //Конструктор без аргументами, викликає конструктор з аргументами шляхом делегування
//    public Student(){
//        this("Іваненко", 1);
//        //System.out.println("Викликано конструктор Student(). Створено "+toString() );
//    }

    //Перерахунок середнього бала по таблиці
    public void updateSerednijBal(){
        serednijBal=0.0;
        int counter=0;
        double sum=0.0;

        for(Map.Entry<String, Double> item: zachotka.entrySet() ){
            sum= sum + item.getValue();
            counter++;
        }

        if( counter>0 ){
            serednijBal= sum/ (double)counter;
        }
    }

    public double getOcinka(String disc){
        if( !zachotka.containsKey( disc ) )
            return 0.0;

        return zachotka.get(disc);
    }

    public int getNumExams(){
        int counter=0;
        for(Map.Entry<String, Double> item: zachotka.entrySet() ) {
            ++counter;
        }

        return counter;
    }
    public void exam( String disc, double ocinka ){
        zachotka.put(disc, ocinka);

        updateSerednijBal();
    }

    //Перетворення об'єкта на рядок
    @Override
    public String toString() {
        return "Student{" +
                "name='" + name + '\'' +
                "("+getStatus() +")" +
                ", kurs=" + kurs +
                ", serednijBal=" + serednijBal +
                ", zachotka=" + zachotka +
                '}'+"\n";
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getKurs() {
        return kurs;
    }

    public void setKurs(int kurs) {
        this.kurs = kurs;
    }

    public double getSerednijBal() {
        return serednijBal;
    }

//    public static Student askStudentParameters()
//    {
//        System.out.println("Введіть параметри об'єкта:");
//        System.out.print("Ім'я:");
//        String name=Main.in.nextLine();
//        System.out.print("Курс:");
//        String s=Main.in.nextLine();
//        int kurs= Integer.parseInt(s);
//        System.out.println();
//        return new Student(name, kurs);
//    }

    // this вчить st
    public void teach(Student st ){
        String disc;
        double from, to;
        for( Map.Entry<String, Double> item :zachotka.entrySet() ) {
            disc = item.getKey();
            to = item.getValue();

            if( st.zachotka.containsKey(disc) ){
                from= st.zachotka.get(disc);
                if(from>=to)continue;
            }
            else from=0.0;

            st.zachotka.put(disc, (from+to)/2.0);

        }

        st.updateSerednijBal();
    }

    //функція, яка порівнює об'єкти за змістом (по замовчуванню - якщо не створювати
    //equals - вона ІСНУЄ! правдв викликається з Object і порівнює адреси об'єктів а не зміст -
    //тобто діє як оператор ==. Рекомендую закоментувати equals в Student і подивитись що вийде.
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Student student = (Student) o;
        return kurs == student.kurs && Objects.equals(name, student.name);
    }

    //перетворює об'єкт на значення int, яке зветься хешкод
    @Override
    public int hashCode() {
        return Objects.hash(name, kurs);
    }

    public void print(){
        System.out.println(toString());
    }

    @Override
    public int compareTo(Student o) {
        //return name.compareTo(o.name);
        if(this.kurs<o.kurs)return -1;

        if(this.kurs>o.kurs)return 1;

        return 0;
    }

    public static class SBComparator implements Comparator<Student>{

        @Override
        public int compare(Student o1, Student o2) {
            if(o1.serednijBal<o2.serednijBal) return -1;
            if(o1.serednijBal>o2.serednijBal) return 1;

            return 0;
        }
    }

    public static int myCompare(Student o1, Student o2 ){
        int ne1=o1.getNumExams();
        int ne2=o2.getNumExams();

        if(ne1<ne2) return -1;
        if(ne1>ne2) return 1;

        return 0;
    }

    public static int uniqueCompare(Student o1, Student o2 ){
        int nameCompare= o1.getName().compareTo( o2.getName() );

        if( nameCompare!=0 )return nameCompare;

        if( o1.getKurs()<o2.getKurs() )return -1;
        if( o1.getKurs()>o2.getKurs() )return 1;

        return 0;
    }

}
