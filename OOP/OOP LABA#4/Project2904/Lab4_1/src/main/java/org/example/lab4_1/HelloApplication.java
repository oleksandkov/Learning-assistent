package org.example.lab4_1;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import javafx.scene.shape.Rectangle;

public class HelloApplication extends Application {
    public static Group group;
    public static Scene scene;
    public static Stage window;

    public static ArrayList<Student> students= new ArrayList<>();

    public static Image imgBachelor;
    public static Image imgMaster;
    public static Image imgPhD;

    public static ImageView imgViewBachelor;
    public static ImageView imgViewMaster;
    public static ImageView imgViewPhD;

    public static Label selected;

    public static Label labelBachelor;
    public static Label labelMaster;
    public static Label labelPhD;

    public static double keyStepX = 10.0;
    public static double keyStepY = 10.0;

    public static Random rnd = new Random();

    public static final int stepMax=3;

    public static double getScreenWX(){
        return window.getWidth();
    }
    public static double getScreenWY(){
        return window.getHeight();
    }

    public static Rectangle findClonePosition(double x, double y, double wx, double wy ){
        x=x+Student.imageDeltaX();
        y=y+Student.imageDeltaY();

        Rectangle r = new Rectangle();
        r.setX(x);
        r.setY(y);
        r.setWidth(wx);
        r.setHeight(wy);

        double [][] deltas = new double[][]{ {1.0,0.0},{1.0, 1.0},{0.0, 1.0},{-1.0, 1.0},
                {-1.0, 0.0},{-1.0, -1.0},{0.0, -1.0},{1.0, -1.0} };

        for( int i=1; i<=stepMax; ++i ){
            for( int j=0; j<8; ++j ){
                double newx = x + wx*1.1*deltas[j][0]*i;
                double newy = y + wy*1.1*deltas[j][1]*i;

                if( newx<0.0 )continue;
                if( newy<0.0 )continue;
                if( !( (newx+wx)<getScreenWX()) )continue;
                if( !( (newy+wy)<getScreenWY()) )continue;

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

            }
        }

        return null;
    }


    public static void copyActive() throws CloneNotSupportedException{

        ArrayList<Student> tmp = new ArrayList<>();

        for (Student r : students) {
            if (r.isActive()) {

                Rectangle rect = findClonePosition( r.getX(), r.getY(),
                        r.getImageWX(), r.getImageWY() );

                if( rect!=null ) {
                    Student newRabbit = (Student)r.clone();
                    newRabbit.setX(rect.getX());
                    newRabbit.setY(rect.getY());
                    newRabbit.resurrect();
                    tmp.add(newRabbit);
                }
            }
            tmp.add(r);
        }

        students=tmp;
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

        if(activate){
            tmp.flipActivation();
            HelloApplication.updateSelected();
        }

        students.add(tmp);

    }

    public static void updateSelected(){
        String namesOfActive="";

        System.out.println("Empty String legnth="+namesOfActive.length());

        boolean isFirst=true;

        for (int i = 0; i < students.size(); ++i) {
            if (students.get(i).isActive()) {
                if(!isFirst) namesOfActive += ",";
                namesOfActive += students.get(i).getName();
                isFirst=false;
            }
        }

        if(namesOfActive.length()>0)namesOfActive= "Selected "+namesOfActive;

        selected.setText(namesOfActive);
    }
    @Override
    public void start(Stage stage) throws IOException {
        //FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        //Scene scene = new Scene(fxmlLoader.load(), 320, 240);

        group= new Group();
        scene = new Scene(group, 1500, 700);
        window = stage;

        imgBachelor= new Image( HelloApplication.class.getResource("student1.png").toString(),
                100,100,false,false);
        System.out.println(HelloApplication.class.getResource("student1.png").toString());
        imgMaster= new Image( HelloApplication.class.getResource("student2.png").toString(),
                100,100,false,false);
        imgPhD= new Image( HelloApplication.class.getResource("student3.png").toString(),
                100,100,false,false);

        /*
        imgViewBachelor = new ImageView(imgBachelor);
        imgViewMaster   = new ImageView(imgMaster);
        imgViewPhD      = new ImageView(imgPhD);

        group.getChildren().addAll(imgViewPhD,  imgViewBachelor, imgViewMaster);

        imgViewPhD.setX(150);
        imgViewPhD.setY(50);

        imgViewMaster.setX(100);
        imgViewMaster.setY(100);

        imgViewBachelor.setX(50);
        imgViewBachelor.setY(150);

          imgViewPhD.setX(50);
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

        //Student []students= new Student[5];

        selected= new Label();
        HelloApplication.group.getChildren().add(selected);
        selected.setLayoutX(0);
        selected.setLayoutY(10);


        for( int i=0; i<5; ++i ){
            Student newStudent= new Student("Obama"+Integer.toString(i+1), i, 75.0+ 10.0*i,
                    rnd.nextDouble() * scene.getWidth(),
                    rnd.nextDouble() * scene.getHeight()
            );

            students.add(newStudent);
            newStudent.resurrect();
        }

        scene.setOnMousePressed(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent mouseEvent) {

                for( int i=0; i<students.size(); ++i ){
                    students.get(i).tryActivate(mouseEvent.getX(),
                            mouseEvent.getY()
                    );
                }

            }
        });



        scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
            @Override
            public void handle(KeyEvent keyEvent) {
                double dx=0.0, dy=0.0;

                switch(keyEvent.getCode()) {
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
                        ArrayList<Student> tmp=new ArrayList<>();
                        for( Student st:students){
                            if( st.isActive() ) st.harakiri();
                            else tmp.add(st);
                        }

                        students= tmp;
                        HelloApplication.updateSelected();
                    }
                    break;
                    case ESCAPE:
                    {
                        for (int i = 0; i < students.size(); ++i) {
                            if (students.get(i).isActive())
                                students.get(i).flipActivation();

                        }
                        HelloApplication.updateSelected();
                    }
                    break;

                    case INSERT:
                    {
                        StudentParamsDlg.display();
                        HelloApplication.updateSelected();
                        break;
                    }

                }

                for( int i=0; i<students.size(); ++i ){
                    if( students.get(i).isActive() )
                        students.get(i).move( dx, dy );
                }

                if(keyEvent.isControlDown()) {
                    if (keyEvent.getCode().equals(KeyCode.V)) {
                        try {
                            HelloApplication.copyActive();
                            HelloApplication.updateSelected();
                        } catch (Exception e) {
                            System.out.println("Cloning failed!");
                        }
                    }
                }


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