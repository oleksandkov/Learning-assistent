package org.example.lab4;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Random;

public class HelloApplication extends Application {
    public static Group group;
    public static Scene scene;

    public static Image imgBachelor;
    public static Image imgMaster;
    public static Image imgPhD;

    public static ImageView imgViewBachelor;
    public static ImageView imgViewMaster;
    public static ImageView imgViewPhD;

    public static Label labelBachelor;
    public static Label labelMaster;
    public static Label labelPhD;

    public static double keyStepX = 10.0;
    public static double keyStepY = 10.0;

    @Override
    public void start(Stage stage) throws IOException {
        //FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        //Scene scene = new Scene(fxmlLoader.load(), 320, 240);

        group= new Group();
        scene = new Scene(group, 1500, 700);

        imgBachelor= new Image( HelloApplication.class.getResource("student1.png").toString(),
                100,100,false,false);
        System.out.println(HelloApplication.class.getResource("student1.png").toString());
        imgMaster= new Image( HelloApplication.class.getResource("student2.png").toString(),
                100,100,false,false);
        imgPhD= new Image( HelloApplication.class.getResource("student3.png").toString(),
                100,100,false,false);

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

        Student []students= new Student[5];

        Random rnd = new Random();

        for( int i=0; i<students.length; ++i ){
            students[i]= new Student("Obama"+Integer.toString(i+1), i, 75.0+ 10.0*i,
                    rnd.nextDouble() * scene.getWidth(),
                    rnd.nextDouble() * scene.getHeight()
            );
            students[i].resurrect();

        }



        scene.setOnMouseClicked(new EventHandler<MouseEvent>() {
                                    @Override
                                    public void handle(MouseEvent mouseEvent) {

                                        for( int i=0; i<students.length; ++i ){
                                            students[i].tryActivate(mouseEvent.getX(),
                                                                 mouseEvent.getY()
                                                                  );
                                        }
                                    }
                                } );

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
                                      }

                                      for( int i=0; i<students.length; ++i ){
                                          if( students[i].isActive() )
                                                     students[i].move( dx, dy );

                                      }

                                  }
                              } );

                stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}