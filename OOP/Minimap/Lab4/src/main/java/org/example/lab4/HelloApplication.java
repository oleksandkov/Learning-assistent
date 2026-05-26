package org.example.lab4;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.SnapshotParameters;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.image.WritableImage;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.transform.Scale;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.*;
import java.util.Arrays;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

public class HelloApplication extends Application {
    public static Group group;
    public static Group group2;
    public static Scene scene;
    public static Stage savedStage;
    public static World world;
    public static double screenWX=1500;
    public static double screenWY=700;
    public static Random rnd = new Random();
    public static boolean animationLocked=false;

    public static ImageView imgviewmap;
    public static Rectangle rectMinimap;
    public static Rectangle rectMinimapScreenProjection;

    public static Image imgBachelor;
    public static Image imgMaster;
    public static Image imgPhD;

    public static Image imgdrugijCorpus;
    public static Image imgfontan;
    public static Image imgguk;
    public static Image imgkitchen;

    public static ImageView imgViewBachelor;
    public static ImageView imgViewMaster;
    public static ImageView imgViewPhD;

    public static Label labelBachelor;
    public static Label labelMaster;
    public static Label labelPhD;

    public static double keyStepX = 10.0;
    public static double keyStepY = 10.0;

    public static void addNewStudent( String name, boolean isActive, int kurs, double health, double x, double y){
        //System.out.printf(name+" "+isActive+" "+kurs+" "+health+" "+x+" "+y);

        world.addNewStudent( new Student(name, kurs, health, x, y ), isActive );

    }

    public static boolean WITHIN( double mx, double my, double x, double y, double wx, double wy ){
        if( mx<x )return false;
        if( my<y )return false;

        if(mx>(x+wx) )return false;
        if(my>(y+wy) )return false;

        return true;
    }

    public static void loadGame(Stage savedStage){
        String currentDir= System.getProperty("user.dir");

        File initDirectory = new File(currentDir);

        FileChooser fileChooser = new FileChooser();

        //Set extension filter
        FileChooser.ExtensionFilter extFilter =
                new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
        fileChooser.getExtensionFilters().add(extFilter);

        fileChooser.setInitialDirectory(
                initDirectory);

        File file = fileChooser.showOpenDialog(savedStage);

        if(file != null){
            //textArea.setText(readFile(file));

            //StringBuilder stringBuffer = new StringBuilder();
            BufferedReader bufferedReader = null;

            try {

                bufferedReader = new BufferedReader(new FileReader(file));

//                            String text;
//                            while ((text = bufferedReader.readLine()) != null) {
//                                stringBuffer.append(text);
//                            }


            world.load(bufferedReader);


            } catch (FileNotFoundException ex) {
                Logger.getLogger(HelloApplication.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(HelloApplication.class.getName()).log(Level.SEVERE, null, ex);
            } finally {
                try {
                    bufferedReader.close();
                } catch (IOException ex) {
                    Logger.getLogger(HelloApplication.class.getName()).log(Level.SEVERE, null, ex);
                }
            }


        }


    }

    public static void saveGame(Stage savedStage){
        String currentDir= System.getProperty("user.dir");

        File initDirectory = new File(currentDir);

        FileChooser fileChooser = new FileChooser();

        //Set extension filter
        FileChooser.ExtensionFilter extFilter =
                new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
        fileChooser.getExtensionFilters().add(extFilter);

        fileChooser.setInitialDirectory(
                initDirectory);


        //Show save file dialog
        //
        File file = fileChooser.showSaveDialog(savedStage);

        if(file != null) {

            try {
                FileWriter fileWriter = null;


                fileWriter = new FileWriter(file);
//--------------------------------------------------------------------------------
                world.save(fileWriter);

                //--------------------------------------------------------------------------------
                fileWriter.close();
            } catch (IOException ex) {
                Logger.getLogger(HelloApplication.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

    }

    @Override
    public void start(Stage stage) throws IOException {
        //FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        //Scene scene = new Scene(fxmlLoader.load(), 320, 240);

        savedStage= stage;

        group= new Group();
        group2=new Group();
        group2.getChildren().add(group);

        Rectangle rect = new Rectangle(World.mapwx, World.mapwy, Color.WHITE);
        group.getChildren().add(rect);

        rectMinimap = new Rectangle(World.mapwx/10.0 +10, World.mapwy/10.0+10, Color.WHITE);
        rectMinimap.setStroke(Color.BLACK);
        rectMinimap.setStrokeWidth(5.0);
        //group.getChildren().add(rectMinimap);

        rectMinimapScreenProjection = new Rectangle(screenWX*World.scaleX,
                screenWY*World.scaleY, Color.RED);
        rectMinimapScreenProjection.setStrokeWidth(2.0);
        rectMinimapScreenProjection.setStroke(Color.RED);
        rectMinimapScreenProjection.setFill(Color.TRANSPARENT);

        scene = new Scene(group2, screenWX, screenWY);

        imgBachelor= new Image( HelloApplication.class.getResource("student1.png").toString(),
                75,75,false,false);
        System.out.println(HelloApplication.class.getResource("student1.png").toString());
        imgMaster= new Image( HelloApplication.class.getResource("student2.png").toString(),
                75,75,false,false);
        imgPhD= new Image( HelloApplication.class.getResource("student3.png").toString(),
                75,75,false,false);

        imgdrugijCorpus= new Image( HelloApplication.class.getResource("DrugijCorpus.jpg").toString(),
                400,300,false,false);
        imgfontan= new Image( HelloApplication.class.getResource("Fontan.jpg").toString(),
                400,300,false,false);
        imgguk= new Image( HelloApplication.class.getResource("GUK.jpg").toString(),
                400,300,false,false);
        imgkitchen= new Image( HelloApplication.class.getResource("Kitchen.jpg").toString(),
                400,300,false,false);

        world= new World(imgdrugijCorpus, imgfontan, imgguk, imgkitchen );

//-------------------------------------------------------------

        scene.setOnMouseClicked(new EventHandler<MouseEvent>() {
                                    @Override
                                    public void handle(MouseEvent mouseEvent) {

                                        double mx= mouseEvent.getX();
                                        double my= mouseEvent.getY();
                                        double coordx= mx - HelloApplication.group.getLayoutX();
                                        double coordy= my - HelloApplication.group.getLayoutY();

                                        if( mouseEvent.getButton().equals(MouseButton.SECONDARY) ) {

                                            //ParamsDlg.display( mouseEvent.getX(), mouseEvent.getY() );
                                            world.rightClickExam(coordx, coordy);

                                        }
                                        else {

                                            if( WITHIN(mx, my, World.minimapX, World.minimapY,
                                                    World.mapwx*World.scaleX, World.mapwy*World.scaleY  ) )
                                            {
                                                double xloc = HelloApplication.group.getLayoutX();
                                                double yloc = HelloApplication.group.getLayoutY();

                                                double xnewloc = (mx-World.minimapX)/World.scaleX ;
                                                double ynewloc = (my-World.minimapY)/World.scaleY ;

                                                if( xnewloc>(World.mapwx-scene.getWidth()) )xnewloc=World.mapwx-scene.getWidth() ;
                                                if( ynewloc>(World.mapwy-scene.getHeight()) )ynewloc=World.mapwy-scene.getHeight();

                                                HelloApplication.group.setLayoutX(-xnewloc);
                                                HelloApplication.group.setLayoutY(-ynewloc);

                                            }
                                            else {
                                                world.tryActivate(coordx, coordy);
                                            }
                                        }

                                        }
                                } );

        scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
                                  @Override
                                  public void handle(KeyEvent keyEvent) {

                                      if (keyEvent.isControlDown()) {
                                          if (keyEvent.getCode().equals(KeyCode.INSERT)) {
                                              //ParamsDlg.display(text.getText(),hexVisible, irabbit.getRotate() );
                                          }

                                          double xloc = HelloApplication.group.getLayoutX();
                                          double yloc = HelloApplication.group.getLayoutY();

                                          double dx = 0.0, dy = 0.0;

                                          switch (keyEvent.getCode()) {
                                              case S:
                                                  animationLocked=true;
                                                  saveGame(savedStage);
                                                  animationLocked=false;
                                                  break;
                                              case O:
                                                  animationLocked=true;
                                                  loadGame(savedStage);
                                                  animationLocked=false;
                                                  break;
                                              case L:
                                                  world.ctrlL();
                                                  break;
                                              case UP:
                                                  dy = -World.screenStep;
                                                  break;
                                              case DOWN:
                                                  dy = +World.screenStep;
                                                  break;
                                              case LEFT:
                                                  dx = -World.screenStep;
                                                  break;
                                              case RIGHT:
                                                  dx = +World.screenStep;
                                                  break;
                                          }

                                          xloc += dx;
                                          yloc += dy;


                                          if(xloc>0)return;
                                          if( xloc<(-1*(World.mapwx-scene.getWidth())) )return;

                                          if(yloc>0)return;
                                          if( yloc<(-1*(World.mapwy-scene.getHeight()) ) )return;

                                          HelloApplication.group.setLayoutX(xloc);
                                          HelloApplication.group.setLayoutY(yloc);

                                          //System.out.println("HelloApplication.group.setLayoutX(xloc):"+xloc);
                                          //System.out.println("HelloApplication.group.setLayoutY(yloc):"+yloc);

                                          //HelloApplication.miniRect.setX(-xloc*0.1);
                                          //HelloApplication.miniRect.setY(-yloc*0.1);

                                      }
                                      else{


                                      double dx = 0.0, dy = 0.0;
                                      boolean toMove=false;

                                      switch (keyEvent.getCode()) {
                                          case UP:
                                              dy = -HelloApplication.keyStepY;
                                              toMove= true;
                                              break;
                                          case DOWN:
                                              dy = HelloApplication.keyStepY;
                                              toMove= true;
                                              break;
                                          case LEFT:
                                              dx = -HelloApplication.keyStepX;
                                              toMove= true;
                                              break;
                                          case RIGHT:
                                              dx = HelloApplication.keyStepX;
                                              toMove= true;
                                              break;
                                          case ESCAPE:
                                              world.esc();
                                              break;
                                          case DELETE:
                                              world.delete();
                                          break;
                                      }

                                      if( toMove ) world.keyMove(dx, dy);

                                  }
                                  }
                              } );



        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long l) {

                if( animationLocked )return;
                world.lifeCycle();


                if( imgviewmap!=null )
                {   group2.getChildren().remove(rectMinimap);
                    group2.getChildren().remove(imgviewmap);
                    group2.getChildren().remove(rectMinimapScreenProjection);
                }
                //saveAsPng( group, "/sample/SnapShot.png");
                final WritableImage SNAPSHOT = group.snapshot(new SnapshotParameters(), null);

                //imgmap = new Image("/sample/SnapShot.png",100,100,false,false);
                imgviewmap=new ImageView(SNAPSHOT);

                //Creating the scale transformation
                Scale scale = new Scale();

                //Setting the dimensions for the transformation
                scale.setX(World.scaleX);
                scale.setY(World.scaleY);

                //System.out.println("proj "+scene.getWidth()*World.scaleX+" "+scene.getHeight()*World.scaleY);

                rectMinimapScreenProjection.setWidth(scene.getWidth()*World.scaleX);
                rectMinimapScreenProjection.setHeight(scene.getHeight()*World.scaleY);

                imgviewmap.setLayoutX(World.minimapX+5);
                imgviewmap.setLayoutY(World.minimapY+5);
                imgviewmap.getTransforms().add(scale);
                rectMinimap.setLayoutX(World.minimapX-5);
                rectMinimap.setLayoutY(World.minimapY-5);


                rectMinimapScreenProjection.setLayoutX(World.minimapX -HelloApplication.group.getLayoutX() * World.scaleX);
                rectMinimapScreenProjection.setLayoutY(World.minimapY - HelloApplication.group.getLayoutY() * World.scaleY );

//                System.out.println("proj " + (World.minimapX -HelloApplication.group.getLayoutX() * World.scaleX) + " "
//                                   + (World.minimapY - HelloApplication.group.getLayoutY() * World.scaleY) );

                group2.getChildren().add(rectMinimap);
                group2.getChildren().add(imgviewmap);
                group2.getChildren().add(rectMinimapScreenProjection);

            }
        };


                stage.setTitle("Hello!");
        stage.setScene(scene);
        timer.start();
        System.out.println("Scene sizes:"+scene.getWidth()+" "+scene.getHeight());
        //stage.setResizable(false);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}