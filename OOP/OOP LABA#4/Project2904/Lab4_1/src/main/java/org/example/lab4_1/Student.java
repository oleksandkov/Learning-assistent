package org.example.lab4_1;

import javafx.geometry.Bounds;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

import java.util.*;

public class Student implements Cloneable, Comparable<Student> {
    private String name;
    private int kurs;
    private double serednijBal;
    private Map<String, Double> zachotka;
    //-------------------------------------------------------
    private Label labelName;

    private Label labelSB;

    private double health;
    private static double healthMax=100;
    private Line life;
    private ImageView istudent;
    private double x, y;

    private double aimx;
    private double aimy;

    private boolean isActive;

    private Rectangle rectActive;


    public static double stipendiaPorig;
    public static double stipendiaKabminPorig;
    public static double akademichnijPorig;

    static{
        setStipendiaPorig(4.0);
        akademichnijPorig=3.0;
        stipendiaKabminPorig=4.5;
        //System.out.println("Викликано статичний блок ініціалізації!");
    }

    public int numberOfSuccessfullyPassedExams(){
        int counter = 0;
        for( Map.Entry<String,Double> record:zachotka.entrySet() ){
            if( ! (record.getValue()<akademichnijPorig) ) ++counter;
        }

        return counter;
    }

    public StudentStatus getStatus(){
        boolean stipendiaKabminFlag= true;
        boolean stipendiaFlag= true;

        for( Map.Entry<String,Double> record:zachotka.entrySet() ){
            if( record.getValue()<akademichnijPorig )return StudentStatus.Debtor;

            if( record.getValue()<stipendiaKabminPorig ) stipendiaKabminFlag= false;
            if( record.getValue()<stipendiaPorig )stipendiaFlag= false;
        }

        if( stipendiaKabminFlag )return StudentStatus.Cabinet_of_Ministers_Scholar;

        if( stipendiaFlag )return StudentStatus.Scholarship_holder;

        return StudentStatus.Regular;
    }

    public static int compareStatus(Student o1, Student o2){
        return o1.getStatus().compareTo(o2.getStatus());
    }

    public boolean perevestiNaNovijKurs(){
        for( Map.Entry<String,Double> record:zachotka.entrySet() ){
            if( record.getValue()<akademichnijPorig )return false;
        }

        ++kurs;
        return true;
    }

    public static double getStipendiaPorig() {
        return stipendiaPorig;
    }

    public static void setStipendiaPorig(double stipendiaPorig) {
        Student.stipendiaPorig = stipendiaPorig;
    }

    public boolean hasStipendia(){
        if(serednijBal<stipendiaPorig)return false;

        return true;
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

    public Bounds getBoundsInParent(){
        return istudent.getBoundsInParent();
    }

    public double getX(){
        return x;
    }
    public double getY(){ return y; }
    public void setX( double _x ){
        x= _x;

        setCoordinates();
    }
    public void setY( double _y ){
        y= _y;

        setCoordinates();
    }

    public double getImageWX(){
        return HelloApplication.imgBachelor.getWidth(); }

    public double getImageWY(){ return HelloApplication.imgBachelor.getHeight();}

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

        labelSB=new Label(Double.toString(this.serednijBal) );
        labelSB.setLayoutX(x+75);
        labelSB.setLayoutY(y);

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

    public void harakiri(){
        HelloApplication.group.getChildren().removeAll(labelName, labelSB, life, istudent);

        if(isActive)HelloApplication.group.getChildren().remove(rectActive);

    }

    public void resurrect(){
        HelloApplication.group.getChildren().addAll(labelName, labelSB, life, istudent);

        if(isActive)HelloApplication.group.getChildren().add(rectActive);
    }

    public boolean flipActivation(){
        if(isActive){
            HelloApplication.group.getChildren().remove(rectActive);
        }
        else{
            HelloApplication.group.getChildren().add(rectActive);
        }
        isActive = !isActive;

        HelloApplication.updateSelected();

        return isActive;
    }


    public boolean tryActivate( double mx, double my ){
        if(istudent.boundsInParentProperty().get().contains(mx,my)){
            flipActivation();
            return true;
        }
        return false;
    }

    public void move( double dx, double dy ){

        x=x+dx;
        y=y+dy;
        setCoordinates();
    }

    public void setCoordinates(){
        labelName.setLayoutX(x);
        labelName.setLayoutY(y);

        labelSB.setLayoutX(x+75);
        labelSB.setLayoutY(y);

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

/*
    public Student(String n, int kurs){
        name = n;
        this.kurs = kurs;

        serednijBal=0.0;

        //System.out.println("Викликано конструктор Student( String n, int kurs). Створено об\'єкт:"+toString());
    }
*/
    /*
    public Student() {
        this("Буш",1);
        //System.out.println("Викликано конструктор Student( ). Створено об\'єкт:"+toString());
        //System.out.println("Викликано конструктор Student( ). Створено об\'єкт:"+toString());
    }
*/
    //функція, яка порівнює об'єкти за змістом (по замовчуванню - якщо не створювати
    //equals - вона ІСНУЄ! правдв викликається з Object і порівнює адреси об'єктів а не зміст -
    //тобто діє як оператор ==. Рекомендую закоментувати equals в Student і подивитись що вийде.
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Student student = (Student) o;
        System.out.println(this.name + " " +this.kurs +" vs "+ student.getName()+" "+student.getKurs());
        return kurs == student.kurs && Objects.equals(name, student.name);
    }

    //перетворює об'єкт на значення int, яке зветься хешкод
    @Override
    public int hashCode() {
        return Objects.hash(name, kurs);
    }

    //копіювальний конструктор
    public Student(Student arg) {
        //arg->this
        this.name = arg.name;
        this.kurs = arg.kurs;

        //this.zachotka = arg.zachotka; //Поверхневе копіювання

        for( Map.Entry<String,Double> record:arg.zachotka.entrySet() ){
            this.zachotka.put(record.getKey(), record.getValue());
        }

        updateSerednijBal();
    }

    @Override
    protected Object clone() throws CloneNotSupportedException {
        Student tmp= (Student)super.clone();

        tmp.zachotka=new HashMap<>();
        //this->tmp
        for( Map.Entry<String,Double> record:this.zachotka.entrySet() ){
            tmp.zachotka.put(record.getKey(), record.getValue());
        }

        tmp.updateSerednijBal();

//--------------------------------------------------------------
        tmp.clearAim();

        tmp.labelSB=new Label(Double.toString(tmp.serednijBal) );
        tmp.labelSB.setLayoutX(tmp.x+75);
        tmp.labelSB.setLayoutY(tmp.y);

        tmp.labelName=new Label(tmp.name);
        tmp.labelName.setLayoutX(tmp.x);
        tmp.labelName.setLayoutY(tmp.y);

        tmp.life=new Line(tmp.x,tmp.y+15, tmp.x+tmp.health,tmp.y+15);
        tmp.life.setStrokeWidth(5);
        tmp.life.setStroke(Color.LIGHTGREEN);

        tmp.istudent= new ImageView(HelloApplication.imgBachelor );
        tmp.istudent.setX(tmp.x+imageDeltaX());
        tmp.istudent.setY(tmp.y+imageDeltaY());

        tmp.isActive=false;

        tmp.rectActive= new Rectangle(tmp.x-5,tmp.y-5,105,105+imageDeltaY());
        tmp.rectActive.setFill(Color.TRANSPARENT);
        tmp.rectActive.setStrokeWidth(3);
        tmp.rectActive.setStroke(Color.YELLOW);

        return tmp;
    }
    public void updateSerednijBal(){
        serednijBal=0.0;
        double sum=0.0;
        int counter=0;

        for( Map.Entry<String,Double> rercord:zachotka.entrySet() ){
            sum += rercord.getValue();
            ++counter;
        }

        if( counter>0 ){
            serednijBal= sum/((double)counter);
        }
    }

    public void exam(String disc, double ocinka ){
        zachotka.put(disc, ocinka);

        updateSerednijBal();
    }

    public double getOcinka(String disc){
        if(! this.zachotka.containsKey(disc))return 0.0;

        for( Map.Entry<String,Double> record:zachotka.entrySet() ) {
            if( record.getKey().equals(disc) )return record.getValue();
        }

        return 0.0;
    }

    public void teach(Student st){
        //this->st
        String disc;
        double to;
        double from;

        for( Map.Entry<String,Double> record:zachotka.entrySet() ){
            disc = record.getKey();
            to = record.getValue();

            if( st.zachotka.containsKey(disc) ){
                from= st.zachotka.get(disc);
                if(from>=to)continue;
            }
            else from=0.0;

            st.exam(disc, (from+to)/2.0 );
        }


    }

    @Override
    public String toString() {
        String rezult =  "Student{" +
                "name='" + name + '\'' +
                ", kurs=" + kurs+
                ", serednijBal=" + String.format("%5.2f ", serednijBal) +
                getStatus()+
                ", zachotka=" + zachotka +
                '}'+'\n';

        return rezult;

    }

    public String getName() {
        return name;
    }

    public int getKurs() {
        return kurs;
    }

    public double getSerednijBal() {
        return serednijBal;
    }


    public void setKurs(int kurs) {
        this.kurs = kurs;
    }

    public void setName(String name) {
        this.name = name;
    }

    {
        //System.out.println("Нестатичний блок ініціалізвції");
        zachotka= new HashMap<String, Double>();
    }
/*
    public static Student askStudentParameters(Scanner in)
    {
        System.out.println("Введіть параметри:");
        System.out.print("Ім'я:");
        String name=in.nextLine();
        name=name.trim();
        System.out.print("Курс:");
        String s=in.nextLine();
        int kurs= Integer.parseInt(s);
        System.out.println();
        return new Student(name, kurs);
    }
*/
    @Override
    public int compareTo(Student o) {  //strcmp(this,o)
        int first = this.name.compareTo(o.name);
        if( first != 0 )return first;

        if( this.kurs < o.kurs )return -1;
        else if( this.kurs > o.kurs )return 1;

        return 0;
    }

    public static class SBComparator implements Comparator<Student>{
        @Override
        public int compare(Student o1, Student o2) { //strcmp(o1.serednijBal,o2.serednijBal) -1 0 +1
            if( o1.serednijBal < o2.serednijBal )return -1;
            else if( o1.serednijBal > o2.serednijBal )return 1;

            return 0;
        }
    }
}
