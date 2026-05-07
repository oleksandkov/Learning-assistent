package org.example.lab4;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class HelloApplication extends Application {
    public static Group group;
    public static Scene scene;
    public static Stage stage;
    public static HelloApplication window;

    public static ArrayList<Student> students=new ArrayList<Student>();

    public static GUK guk;
    public static DrugijCorpus drugijCorpus;
    public static Fontan fontan;

    public static Image imgBachelor;
    public static Image imgMaster;
    public static Image imgPhD;

    public static Image imgdrugijCorpus;
    public static Image imgfontan;
    public static Image imgguk;

    public static ImageView imgViewBachelor;
    public static ImageView imgViewMaster;
    public static ImageView imgViewPhD;

    public static Label labelBachelor;
    public static Label labelMaster;
    public static Label labelPhD;

    public static double keyStepX = 10.0;
    public static double keyStepY = 10.0;

    public static final String math_course="Математика";
    public static final String physics_course="Фізика";
    public static final String oop_course="ООП";

    public static Random rnd = new Random();

    public static void
    adjustStudent(Student student, String name, int kurs, double health,
                  boolean activate, double _x, double _y){

        student.setName(name);
        student.setKurs(kurs);
        student.setHealth(health);

        if( activate ){
            if( !student.isActive() )student.flipActivation();
        }else{
            if( student.isActive() )student.flipActivation();
        }

        student.setX(_x);
        student.setY(_y);

    }

    public static void
    addNewStudent(String name, int kurs, double health,
                                     boolean activate, double _x, double _y){

        if( (_x == 0.0) || (_y == 0.0) ){
            _x=rnd.nextDouble()*scene.getWidth() ;
            _y=rnd.nextDouble()*scene.getHeight() ;
        }

        Student tmp = new Student(name, kurs, health, _x, _y );

        tmp.resurrect();

        if(activate)tmp.flipActivation();

        students.add(tmp);

    }


    public static void updateMacros(){
        guk.updateCache();
        drugijCorpus.updateCache();
        fontan.updateCache();

        guk.clearBelong();
        drugijCorpus.clearBelong();
        fontan.clearBelong();
        for( Student st:students ){
            guk.verifyBelong(st);
            drugijCorpus.verifyBelong(st);
            fontan.verifyBelong(st);
        }
        guk.updateBelong();
        drugijCorpus.updateBelong();
        fontan.updateBelong();


    }

    public double getScreenWX(){
        return HelloApplication.stage.getWidth();
    }

    public double getScreenWY(){
        return HelloApplication.stage.getHeight();
    }

    public Rectangle findClonePosition(double x, double y, double wx, double wy ){
        x=x+Student.imageDeltaX();
        y=y+Student.imageDeltaY();

        //System.out.println("findClonePosition="+x+" "+y+" "+wx+" "+wy);

        Rectangle r = new Rectangle();
        r.setX(x);
        r.setY(y);
        r.setWidth(wx);
        r.setHeight(wy);

        int step=1;
        double dwx = 20.0;
        double dwy = 20.0;
        do{
            double newx = x + dwx*step;
            double newy = y + dwy*step;
            ++step;

            if( newx<0.0 )return null;
            if( newy<0.0 )return null;
            if( !( (newx+wx)<getScreenWX()) )return null;
            if( !( (newy+wy)<getScreenWY()) )return null;

            r.setX(newx);
            r.setY(newy);

            boolean found=false;
            for (Student st : students) {

                if( st.getBoundsInParent().intersects( r.getBoundsInParent() )  ){
                    found=true;
                    break;
                }

            }

            if( !found ){
                r.setX(newx-Student.imageDeltaX());
                r.setY(newy-Student.imageDeltaY());

                return r;

            }

        }while(true);

    }


    @Override
    public void start(Stage stage) throws IOException {
        //FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        //Scene scene = new Scene(fxmlLoader.load(), 320, 240);

        window = this;
        HelloApplication.stage=stage;
        group= new Group();
        scene = new Scene(group, 1500, 800);

        imgBachelor= new Image( HelloApplication.class.getResource("student1.png").toString(),
                100,100,false,false);
        System.out.println(HelloApplication.class.getResource("student1.png").toString());
        imgMaster= new Image( HelloApplication.class.getResource("student2.png").toString(),
                100,100,false,false);
        imgPhD= new Image( HelloApplication.class.getResource("student3.png").toString(),
                100,100,false,false);

        imgdrugijCorpus= new Image( HelloApplication.class.getResource("DrugijCorpus.jpg").toString(),
                400,300,false,false);
        imgfontan= new Image( HelloApplication.class.getResource("Fontan.jpg").toString(),
                400,300,false,false);
        imgguk= new Image( HelloApplication.class.getResource("GUK.jpg").toString(),
                400,300,false,false);



//        imgViewBachelor = new ImageView(imgBachelor);
//        imgViewMaster   = new ImageView(imgMaster);
//        imgViewPhD      = new ImageView(imgPhD);
//
//        group.getChildren().addAll(imgViewPhD, imgViewMaster, imgViewBachelor);

//        imgViewPhD.setX(150);
//        imgViewPhD.setY(50);
//
//        imgViewMaster.setX(100);
//        imgViewMaster.setY(100);
//
//        imgViewBachelor.setX(50);
//        imgViewBachelor.setY(150);

/*        imgViewPhD.setX(50);
        imgViewPhD.setY(50);

        imgViewMaster.setX(200);
        imgViewMaster.setY(50);

        imgViewBachelor.setX(350);
        imgViewBachelor.setY(50);


    labelBachelor= new Label("Bachelor");
    labelMaster= new Label("Master");;
    labelPhD= new Label("PhD");;

        group.getChildren().addAll(labelBachelor, labelMaster, labelPhD );

        labelBachelor.setLayoutX(350);
        labelBachelor.setLayoutY(30);
        labelMaster.setLayoutX(200);
        labelMaster.setLayoutY(30);
        labelPhD.setLayoutX(50);
        labelPhD.setLayoutY(30);
 */

//-------------------------------------------------------------
        //Student(String n, int kurs, double h, double _x, double _y )

        guk= new GUK(imgguk, 50, 50);
        drugijCorpus = new DrugijCorpus(imgdrugijCorpus, 1100, 100 );
        fontan = new Fontan(imgfontan, 300,400 );


        for( int i=0; i<5; ++i ){
            students.add( new Student("Obama"+Integer.toString(i+1), i, 75.0+ 10.0*i,
                    rnd.nextDouble() * scene.getWidth(),
                    rnd.nextDouble() * scene.getHeight()) );
            students.get(i).resurrect();

        }

        updateMacros();

        scene.setOnMouseClicked(new EventHandler<MouseEvent>() {
                                    @Override
                                    public void handle(MouseEvent mouseEvent) {

                                        if( mouseEvent.getButton().equals(MouseButton.SECONDARY) ){
                                            for (int i = 0; i < students.size(); ++i){
                                                if(students.get(i).containsMouse(mouseEvent.getX(), mouseEvent.getY()) ){
//                                                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
//                                                    alert.setTitle("Student");
//                                                    alert.setHeaderText("Parameters");
//                                                    alert.setContentText(students.get(i).toString());
//                                                    alert.showAndWait();


                                                    StudentChangeParamsDlg.display(students.get(i));
                                                    break;

                                                }
                                            }

                                        }
                                        else {


                                            boolean rez = false;
                                            for (int i = 0; i < students.size(); ++i) {
                                                rez = rez || students.get(i).tryActivate(mouseEvent.getX(),
                                                        mouseEvent.getY()
                                                );
                                            }
                                            if (rez) {
                                               // System.out.println("mouse updateMacros();");
                                                updateMacros();
                                            }
                                        }
                                    }
                                });

                scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
                    @Override
                    public void handle(KeyEvent keyEvent) {

                        double dx = 0.0, dy = 0.0;

                        System.out.println("Pressed "+keyEvent.getCode());

                        if(keyEvent.isControlDown()) {
                            if (keyEvent.getCode().equals(KeyCode.C)) {
                                try {
                                    ArrayList<Student> active_array= new ArrayList<Student>();

                                    for (int i = 0; i < students.size(); ++i) {
                                        if (students.get(i).isActive() ) active_array.add(students.get(i));
                                    }

                                    for (Student ast:active_array ) {
                                        Rectangle rect = findClonePosition( ast.getX(), ast.getY(),
                                                ast.getImageWX(), ast.getImageWY() );

                                        if( rect!=null ) {
                                            Student newStudent = (Student)ast.clone();
                                            newStudent.setX(rect.getX());
                                            newStudent.setY(rect.getY());
                                            //System.out.println("Created rabbit="+newRabbit);
                                            newStudent.resurrect();
                                            students.add(newStudent);
                                        }

                                    }
                                } catch (Exception e) {
                                    System.out.println("Cloning failed!");
                                }
                            }

                            updateMacros();
                            return;
                        }

                        switch (keyEvent.getCode()) {
                            case UP:
                                dy = -HelloApplication.keyStepY;
                                break;
                            case DOWN:
                                dy = HelloApplication.keyStepY;
                                break;
                            case LEFT:
                                dx = -HelloApplication.keyStepX;
                                break;
                            case RIGHT:
                                dx = HelloApplication.keyStepX;
                                break;
                            case DELETE:
                            {
                                ArrayList<Student> tmp=new ArrayList<Student> ();
                                for (int i = 0; i < students.size(); ++i) {
                                    if (students.get(i).isActive()){
                                        students.get(i).harakiri();
                                    }
                                    else tmp.add(students.get(i));

                                }

                                students = tmp;

                            }
                            break;
                            case ESCAPE: {
                                for (int i = 0; i < students.size(); ++i) {
                                    if (students.get(i).isActive()) {
                                        students.get(i).flipActivation();
                                    }
                                }
                            }
                            break;
                            case INSERT:
                                StudentParamsDlg.display();
                                break;
                            case M:
                                for (int i = 0; i < students.size(); ++i) {
                                    if (students.get(i).isActive()){
                                        double math_grade = students.get(i).getOcinka(math_course);
                                        math_grade= math_grade + 0.5;
                                        if( !(math_grade>5.5))students.get(i).exam(math_course,math_grade);
                                        System.out.println(students.get(i));
                                    }
                                }
                                break;
                            case P:
                                for (int i = 0; i < students.size(); ++i) {
                                    if (students.get(i).isActive()){
                                        double physics_grade = students.get(i).getOcinka(physics_course);
                                        physics_grade= physics_grade + 0.5;
                                        if( !(physics_grade>5.5))students.get(i).exam(physics_course,physics_grade);
                                        System.out.println(students.get(i));
                                    }
                                }
                                break;

                            case O:
                                for (int i = 0; i < students.size(); ++i) {
                                    if (students.get(i).isActive()){
                                        double oop_grade = students.get(i).getOcinka(oop_course);
                                        oop_grade= oop_grade + 0.5;
                                        if( !(oop_grade>5.5))students.get(i).exam(oop_course,oop_grade);
                                        System.out.println(students.get(i));
                                    }
                                }
                                break;

                        }

                        for (int i = 0; i < students.size(); ++i) {
                            if (students.get(i).isActive())
                                students.get(i).move(dx, dy);

                        }

                        updateMacros();
                    }
                });

                stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}