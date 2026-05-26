package org.example.lab4;

import javafx.scene.image.Image;

import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class World {

    public static double mapwx=3000.0;
    public static double mapwy=2000.0;

    public static double scaleX=0.1;
    public static double scaleY=0.1;

    public static double minimapX= 100;
    public static double minimapY= 470;

    public static double screenStep=50.0;

    private DrugijCorpus drugijCorpus;
    private Fontan fontan;
    private GUK guk;
    private Kitchen kitchen;

    private ArrayList<Student> students = new ArrayList<>();

    private Image _imgdrugijCorpus;
    private Image _imgfontan;
    private Image _imgguk;
    private Image _imgkitchen;

    public World(Image imgdrugijCorpus, Image imgfontan, Image imgguk, Image imgkitchen ){

    _imgdrugijCorpus=imgdrugijCorpus;
    _imgfontan=imgfontan;
    _imgguk=imgguk;
    _imgkitchen=imgkitchen;

        fontan= new Fontan(imgfontan,400.0,1200.0);
        guk = new GUK(imgguk, 100.0, 120.0);
        drugijCorpus = new DrugijCorpus(imgdrugijCorpus, 2000.0, 100.0 );
        kitchen = new Kitchen(imgkitchen, 1000.0, 1000.0 );

        //----------------------------------------------



        for( int i=0; i<15; ++i ){
            Student st= new Student("Obama"+Integer.toString(i+1), i, 75.0+ 3.0*i,
                    HelloApplication.rnd.nextDouble() * HelloApplication.scene.getWidth(),
                    HelloApplication.rnd.nextDouble() * HelloApplication.scene.getHeight()
            );

            students.add(st);

            st.resurrect();

        }
        //----------------------------------------------


    }

    public Student askStudAddress( int idx ){
        return students.get(idx);
    }

    public int askStudNumber(Student st ){
        for( int i=0; i<students.size(); ++i ){
            if( students.get(i) == st )return i;
        }
        System.out.println("Student not found!");
        return -1;
    }
    public static int askWorldStudNumber(Student st){
        return HelloApplication.world.askStudNumber(st);
    }

    public static Student askWorldStudAddress( int idx ){
        return HelloApplication.world.askStudAddress(idx);
    }
    public void save(FileWriter fileWriter ) throws IOException {

        fileWriter.write( Integer.toString(students.size()) + "\n" );
        for( Student st:students) st.save(fileWriter);

        fontan.save(fileWriter);
        guk.save(fileWriter);
        drugijCorpus.save(fileWriter);
        kitchen.save(fileWriter);
    }

    public void load(BufferedReader bufferedReader) throws IOException {

        for( int i=0; i<students.size(); ++i )
            students.get(i).harakiri();

        students=new ArrayList<>();

        fontan= new Fontan(_imgfontan,400.0,1200.0);
        guk = new GUK(_imgguk, 100.0, 120.0);
        drugijCorpus = new DrugijCorpus(_imgdrugijCorpus, 2000.0, 100.0 );
        kitchen = new Kitchen(_imgkitchen, 1000.0, 1000.0 );

        String text;
        text = bufferedReader.readLine();
        int hmany= Integer.parseInt(text);

        for( int i=0; i<hmany; ++i ){
            String name=bufferedReader.readLine();

            text = bufferedReader.readLine();
            int kurs =Integer.parseInt(text);

            text = bufferedReader.readLine();
            double health=Double.parseDouble(text);

            text = bufferedReader.readLine();
            double x= Double.parseDouble(text);

            text = bufferedReader.readLine();
            double y= Double.parseDouble(text);

            Student st= new Student(name, kurs, health, x, y);

            students.add(st);
            st.resurrect();

            st.load(bufferedReader);
        }


        fontan.load(bufferedReader);
        guk.load(bufferedReader);
        drugijCorpus.load(bufferedReader);
        kitchen.load(bufferedReader);


    }

    public void freeStables(Student st){

        if( fontan.freePlaces(st) )return;
        if( guk.freePlaces(st) )return;
        if( drugijCorpus.freePlaces(st) )return;
        if( kitchen.freePlaces(st) )return;

    }

    public static void askWorldFreeStables(Student st){
        HelloApplication.world.freeStables(st);
    }

    public void ctrlL(){
        //System.out.println("Ctrl_L!!!");
        for (Student st:students ){
            if ( st.isActive() ){
                st.flipActivation();

                if(st.getProcessing()){
                    freeStables(st);
                    st.clearProcessing();
                }

                drugijCorpus.aimCar(st);
            }

        }
    }

    public void keyMove(double dx, double dy){

        for (Student st:students ) {
            if ( st.isActive())
                st.keyMove(dx, dy);
        }
    }

    public void delete(){
        ArrayList<Student> tmp = new ArrayList<>();

        for (Student st:students) {
            if (st.isActive()) st.harakiri();
            else tmp.add(st);
        }

        students = tmp;
    }

    public void addNewStudent( Student st, boolean isActive ){
        st.resurrect();
        st.setActive(isActive);
        students.add(st);
    }

    public void esc(){
        for (Student st:students) {
            if( st.isActive() )
                st.flipActivation();
        }
    }

    public void tryActivate(double coordx, double coordy){
        for (Student st:students)
            st.tryActivate(coordx, coordy);
    }

    public void rightClickExam(double coordx, double coordy){
//        for (Student st:students) {
//            if( st.touched( coordx, coordy ) ) {
//                st.exam(DEBUG_disc[DEBUG_didx++], (double) DEBUG_grade++);
//
//                if(DEBUG_didx==DEBUG_disc.length) DEBUG_didx=0;
//                if(DEBUG_grade>5)DEBUG_grade=1;
//            }
//        }

        for (Student st:students)
            if( st.touched( coordx, coordy ) ) System.out.println(st);

    }

    public void lifeCycle(){
        drugijCorpus.lifeCycle();

        for( Student st:students ) {
            st.lifeCycle();
            st.autoMove();
        }
        //rabbitHole.lifeCycle();
        //cornField.lifeCycle();
    }

    public static void askWorldToFree(Student st){
        HelloApplication.world.toFree(st);
    }

    public static void askWorldwhatToDo(Student st){
        HelloApplication.world.whatToDo(st);
    }

    public void toFree(Student st){
        freeStables(st);
        st.clearAim();

        Location []arr = new Location[]{drugijCorpus, fontan, guk, kitchen};
        int next= HelloApplication.rnd.nextInt(4);

        arr[next].aimCar(st);

    }

    public void whatToDo(Student st){

        //if( rabbitHole.worksWith(rbt) )return;
        //if( cornField.worksWith(rbt) )return;

        if( drugijCorpus.worksWith(st) )return;
        if( fontan.worksWith(st) )return;
        if( guk.worksWith(st) )return;
        if( kitchen.worksWith(st) )return;

        Location []arr = new Location[]{drugijCorpus, fontan, guk, kitchen};
        int next= HelloApplication.rnd.nextInt(4);

        arr[next].aimCar(st);
        //if( rbt.isHungry() ){
        //    cornField.aimCar(rbt);
        //    return;
        //}
        //rabbitHole.aimCar(rbt);
    }


    public static void planningSwitch(Student st, Location loc){
        if( loc instanceof DrugijCorpus )HelloApplication.world.planning(st, Operations.JUSTSTOP);
        else HelloApplication.world.planning(st, Operations.JUSTSTOP);
    }

    public void planning(Student st, Operations op)
    {
        st.clearProcessing();
        switch(op)
        {
            case JUSTSTOP: {
                double x = HelloApplication.rnd.nextDouble() * HelloApplication.scene.getWidth();
                double y = HelloApplication.rnd.nextDouble() * HelloApplication.scene.getHeight();
                st.setAim(x, y);
                }
                break;
            default:
                break;
        }

    }

    public static int DEBUG_grade=1;
    public static String [] DEBUG_disc=new String[]{"Математика","ООП", "МЗПІ", "Фізика"};
    public static int DEBUG_didx=0;

    public static String askWorldNextCourse(){
        String s= DEBUG_disc[DEBUG_didx];

        ++DEBUG_didx;
        if( DEBUG_didx==DEBUG_disc.length )DEBUG_didx=0;

        return s;
    }

}
