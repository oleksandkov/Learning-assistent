package sample;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.image.Image;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.KeyCode;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.stage.Stage;

import java.io.IOException;

public class Main extends Application {


    public static World world;

    public static Group group;
    public static Group group2;
    public static Scene scene;

    public static Label strDollars;
    public static Label strSteel;
    public static Label strOre;
    public static Label strCoal;


    public static int count = 0;

    private static double screenStep=50.0;
    private static boolean gflg=false;

    @Override
    public void start(Stage primaryStage) throws Exception{
        Parent root = FXMLLoader.load(getClass().getResource("sample.fxml"));
        primaryStage.setTitle("Hello World");


        ProgressIndicator pi;

        group = new Group();
        group2= new Group();

        Rectangle rect = new Rectangle(World.mapwx, World.mapwy, Color.WHITE);
        group.getChildren().add(rect);

        Image imgshahta;
        imgshahta = new Image("/sample/Shahta.jpg");

        Image imggok;
        imggok = new Image("/sample/GOK.jpg");

        Image imgzavod;
        imgzavod = new Image("/sample/zavod.jpg");

        Image imgbaza;
        imgbaza = new Image("/sample/mbaza.jpg");

        Image imgidle;
        imgidle= new Image("/sample/idle.png");

        LightCar.imgLightCar= new Image("/sample/car1.png");

        LightCar.imgNone= new Image("/sample/none.png");
        LightCar.imgCoal= new Image("/sample/coal.png");
        LightCar.imgOre= new Image("/sample/ore.png");
        LightCar.imgSteel= new Image("/sample/steel.png");


        MediumCar.imgMediumCar= new Image("/sample/car2.png");
        HeavyCar.imgHeavyCar= new Image("/sample/car3.png");

        world= new World(imgshahta, imggok, imgzavod, imgbaza, imgidle );


        group2.getChildren().add(group);

        strDollars = new Label();
        strDollars.setFont(new Font(24));
        strSteel = new Label();
        strSteel.setFont(new Font(24));
        strOre = new Label();
        strOre.setFont(new Font(24));
        strCoal = new Label();
        strCoal.setFont(new Font(24));

        group2.getChildren().addAll(strDollars,strCoal,strOre,strSteel);

        strCoal.setLayoutX(5);
        strCoal.setLayoutY(5);
        strOre.setLayoutX(5);
        strOre.setLayoutY(25);
        strSteel.setLayoutX(5);
        strSteel.setLayoutY(45);
        strDollars.setLayoutX(5);
        strDollars.setLayoutY(65);

        scene = new Scene(group2, primaryStage.getWidth(), primaryStage.getHeight());

        primaryStage.setScene(scene);

        scene.setOnMousePressed(new EventHandler<MouseEvent>() {
                                    @Override
                                    public void handle(MouseEvent event) {

                                        double x=event.getX()-Main.group.getLayoutX();
                                        double y=event.getY()-Main.group.getLayoutY();

//                                        System.out.println("Mouse: "+Main.group.getLayoutX()+
//                                                " "+ Main.group.getLayoutY()+
//                                                " X:" +event.getX() +" Y:"+ event.getY());

                                        if ( event.getButton() == MouseButton.SECONDARY ) {

                                            if ( event.isControlDown() )
                                            {world.coalLoad(x,y);
//                                                System.out.println("Coal");
                                            }
                                            else if ( event.isAltDown() )
                                            {  world.oreLoad(x,y);
//                                                System.out.println("Ore");
                                            }
                                                  else if ( event.isShiftDown() )
                                                  { world.steelLoad(x,y);
//                                                      System.out.println("Steel");
                                                  }


                                        }

                                        if ( event.getButton() == MouseButton.PRIMARY ) {
                                            world.activate(x,y);
                                        }


                                    }
                                }
        );


        scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
            @Override
            public void handle(KeyEvent event) {



                if (event.isControlDown())
                {
                    boolean operated=false;
                    switch(event.getCode()) {
                        case DIGIT1:
                            world.generateCar(CarLevel.LIGHT);operated=true;
                            break;
                        case DIGIT2:
                            world.generateCar(CarLevel.MEDIUM);operated=true;
                            break;
                        case DIGIT3:
                            world.generateCar(CarLevel.HEAVY);operated=true;
                            break;
                        case DIGIT4:
                            world.outputActivatedCars();operated=true;
                            break;
                        case R:
                            world.toggleRestMode();operated=true;
                            break;
                        case U:
                            world.unloadActive();
                            break;
                        case I:
                            world.installActivated();
                            break;

                    }

                    if(operated)return;

                    double xloc= Main.group.getLayoutX();
                    double yloc = Main.group.getLayoutY();

                    switch(event.getCode()) {
                    case UP:
                            yloc -= Main.screenStep;
                            break;
                        case DOWN:
                            yloc += Main.screenStep;
                            break;
                        case LEFT:
                            xloc -= Main.screenStep;
                            break;
                        case RIGHT:
                            xloc += Main.screenStep;
                            break;
                    }

                    if(xloc>0)xloc=0.0;
                    if(yloc>0)yloc=0.0;

                    Main.group.setLayoutX(xloc);
                    Main.group.setLayoutY(yloc);


                }
                else
                {
                    double x = 0;
                    double y = 0;

                    switch (event.getCode()) {
                        case UP:
                            y -= World.carstepdelta;
                            world.carKeyMove(x,y);
                            break;
                        case DOWN:
                            y += World.carstepdelta;
                            world.carKeyMove(x,y);
                            break;
                        case LEFT:
                            x -= World.carstepdelta;
                            world.carKeyMove(x,y);
                            break;
                        case RIGHT:
                            x += World.carstepdelta;
                            world.carKeyMove(x,y);
                            break;
                    }
                }

            }
        });


        AnimationTimer timer = new AnimationTimer() {

            @Override
            public void handle(long now) {

                if( !gflg ) {
                    world.generateCar(CarLevel.LIGHT);
                    world.generateCar(CarLevel.MEDIUM);
                    world.generateCar(CarLevel.HEAVY);
                    world.generateCar(CarLevel.LIGHT);
                    world.generateCar(CarLevel.MEDIUM);
                    world.generateCar(CarLevel.HEAVY);
                    world.generateCar(CarLevel.LIGHT);
                    world.generateCar(CarLevel.MEDIUM);
                    world.generateCar(CarLevel.HEAVY);
                    gflg=true;
                }

                String text= "Dollars earned:"+Integer.toString(World.dollarsEarned) ;
                strDollars.setText(text);

                text= "Coal mined:"+World.coalMined;
                strCoal.setText(text);

                text="Ore mined:"+World.oreMined;
                strOre.setText(text);

                text="Steel smelted:"+World.steelSmelted;
                strSteel.setText(text);

                world.lifeCycle();
            }

        };

        timer.start();

        primaryStage.show();
    }


    public static void main(String[] args) {
        launch(args);
    }
}
