package com.example.lab3;
import javafx.animation.AnimationTimer;
import javafx.event.EventHandler;
import javafx.application.Application;
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
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.transform.Scale;
import javafx.stage.Stage;

import java.io.IOException;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.ArrayList;

public class HelloApplication extends Application {

    private static double screenStep=50.0;
    public static Group group= new Group();
    public static Group group2= new Group();

    public static Image imgrabbit;
    public static Image imgwolf;
    public static Image imgcarrot;
    public static Image imgcornField;
    public static Image imgGarden;
    public static Image imgRabbitHole;

    public static Rectangle activeAreaRect;
    public static ImageView imgviewmap;
    public static boolean imgviewmap_firstTime = true;
    static LocalDateTime beginTime = LocalDateTime.now();
    static int frames=0;
    static Label label= new Label();
    //    public static int counter=0;

    public static double minimapScale = 0.125;
    public static double minimapBaseX = 1115.0;
    public static double minimapBaseY = 3.0;
    public static Rectangle minimapBorderRect;
    public static World world;

    public static Stage stage;

    @Override
    public void start(Stage stage) throws IOException, CloneNotSupportedException {

        HelloApplication.stage = stage;
        //FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        //Scene scene = new Scene(fxmlLoader.load(), 320, 240);
        //stage.setTitle("Hello!");

        //rabbit.png
        imgrabbit= new Image( HelloApplication.class.getResource("rabbit.png").toString(),
                50,50,false,false);

        imgwolf= new Image( HelloApplication.class.getResource("wolf.png").toString(),
                100,100,false,false);


        imgcarrot= new Image( HelloApplication.class.getResource("carrot.png").toString(),
                30,30,false,false);
        imgcornField = new Image( HelloApplication.class.getResource("CornField.png").toString(),
                400,300,false,false);
        imgRabbitHole = new Image( HelloApplication.class.getResource("RabbitHole.png").toString(),
                400,250,false,false);

        imgGarden = new Image( HelloApplication.class.getResource("Garden.jpg").toString(),
                400,150,false,false);

        Rectangle rect = new Rectangle(World.mapwx, World.mapwy, Color.WHITE);
        group.getChildren().add(rect);


        group2.getChildren().add(group);
        group2.getChildren().add(label);
        label.setLayoutX(0);
        label.setLayoutY(50);

        Scene scene = new Scene(group2, 1500, 800);
        //Scene scene = new Scene(group, stage.getWidth(), stage.getHeight() );

        scene.setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent mouseEvent) {
//                Rabbit r= new Rabbit(   Integer.toString(++counter), 100.0,  mouseEvent.getX(), mouseEvent.getY() );
//                herd.add(r);

                if(minimapBorderRect.contains( mouseEvent.getX(), mouseEvent.getY() )){
                    double virtualX= ( mouseEvent.getX()-HelloApplication.minimapBaseX )
                            /HelloApplication.minimapScale ;
                    double virtualY= ( mouseEvent.getY()-HelloApplication.minimapBaseY )
                            /HelloApplication.minimapScale ;

                    if( !( (virtualX + stage.getWidth()) < World.mapwx ) ){
                        virtualX = World.mapwx - stage.getWidth();
                    }

                    if( !( (virtualY + stage.getHeight() ) < World.mapwy ) ){
                        virtualY = World.mapwy - stage.getHeight();
                    }

                    HelloApplication.group.setLayoutX(-1*virtualX);
                    HelloApplication.group.setLayoutY(-1*virtualY);

                    return;
                }

                double xloc= -1*HelloApplication.group.getLayoutX() + mouseEvent.getX();
                double yloc = -1*HelloApplication.group.getLayoutY() + mouseEvent.getY();


                //double virtualX=


                if( mouseEvent.getButton().equals(MouseButton.SECONDARY) ){
                    ChooseRabbitToChangeParamsDlg.display(xloc, yloc);
                }
                else{
                    boolean flg=world.mousePrimaryActivate(xloc, yloc);


                    if( flg==false)
                        RabbitParamsDlg.display(xloc, yloc);
                }
                //System.out.println("Got control back!");
            }
        });

        scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
            @Override
            public void handle(KeyEvent keyEvent) {

                if(keyEvent.isControlDown()) {

                    double xloc= HelloApplication.group.getLayoutX();
                    double yloc = HelloApplication.group.getLayoutY();

                    switch(keyEvent.getCode()) {
                        case UP:
                            yloc -= HelloApplication.screenStep;
                            break;
                        case DOWN:
                            yloc += HelloApplication.screenStep;
                            break;
                        case LEFT:
                            xloc -= HelloApplication.screenStep;
                            break;
                        case RIGHT:
                            xloc += HelloApplication.screenStep;
                            break;
                    }

                    if(xloc>0)xloc=0.0;

                    if( !( (-1*xloc + stage.getWidth()) < World.mapwx ) ){
                        xloc = -1*(World.mapwx - stage.getWidth());
                    }

                    if(yloc>0)yloc=0.0;

                    if( !( (-1*yloc + stage.getHeight() ) < World.mapwy ) ){
                        yloc = -1*(World.mapwy - stage.getHeight());
                    }


                    HelloApplication.group.setLayoutX(xloc);
                    HelloApplication.group.setLayoutY(yloc);


                }



                    if(keyEvent.isControlDown()) {
                    if (keyEvent.getCode().equals(KeyCode.V) ){
                            try {
                                world.copyActive();
                            }
                            catch(Exception e){
                                System.out.println("Cloning failed!");
                            }
                    }

                    if (keyEvent.getCode().equals(KeyCode.I) ){
                        world.installActivated();
                    }

                }


                    if (keyEvent.getCode().equals(KeyCode.DELETE) )
                {
                    world.deleteActive();
                }

                boolean flg=false;
                double dx=0.0;
                double dy=0.0;

                if (keyEvent.getCode().equals(KeyCode.W) )
                {
                    dy=-10.0; flg= true;
                }
                if (keyEvent.getCode().equals(KeyCode.A) )
                {
                    dx=-10.0; flg= true;
                }
                if (keyEvent.getCode().equals(KeyCode.S) )
                {
                    dy=10.0; flg= true;
                }
                if (keyEvent.getCode().equals(KeyCode.D) )
                {
                    dx=10.0; flg= true;
                }

                if( flg ){
                    world.moveActive(dx, dy);
                }

            }
        });

        stage.setScene(scene);
        stage.show();

        world= new World(this, group, imgrabbit, imgwolf,
                imgcarrot, imgcornField, imgRabbitHole, imgGarden );

        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long l) {
                    world.lifeCycle();

                LocalDateTime nextTime= LocalDateTime.now();
                if( ChronoUnit.SECONDS.between( beginTime,nextTime)>0 )
                {
                    label.setText(Integer.toString(frames));
                    frames=0;
                    beginTime= nextTime;
                    //-------------------------------------------------------------------------


                    if( imgviewmap!=null )
                    {
                        group2.getChildren().remove(imgviewmap);
                        group2.getChildren().remove(activeAreaRect);
                    }
                    else{
                        if(imgviewmap_firstTime){
                            imgviewmap_firstTime=false;

                            minimapBorderRect=new Rectangle(World.mapwx*HelloApplication.minimapScale+6.0,
                                    World.mapwy*HelloApplication.minimapScale+6.0);
                            minimapBorderRect.setFill(Color.TRANSPARENT);
                            minimapBorderRect.setStrokeWidth(3);
                            minimapBorderRect.setStroke(Color.BLACK);

                            minimapBorderRect.setX(HelloApplication.minimapBaseX-3.0);
                            minimapBorderRect.setY(HelloApplication.minimapBaseY-3.0);
                            group2.getChildren().add(minimapBorderRect);

                            //System.out.println( "Screen sies="+HelloApplication.stage.getWidth()+" "+HelloApplication.stage.getHeight() );
                        }
                    }
                    //saveAsPng( group, "/sample/SnapShot.png");
                    final WritableImage SNAPSHOT = group.snapshot(new SnapshotParameters(), null);

                    //imgmap = new Image("/sample/SnapShot.png",100,100,false,false);
                    imgviewmap=new ImageView(SNAPSHOT);
                    group2.getChildren().add(imgviewmap);

                    //Creating the scale transformation
                    Scale scale = new Scale();

                    //Setting the dimensions for the transformation
                    scale.setX(HelloApplication.minimapScale);
                    scale.setY(HelloApplication.minimapScale);

                    imgviewmap.setLayoutX(HelloApplication.minimapBaseX);
                    imgviewmap.setLayoutY(HelloApplication.minimapBaseY);
                    imgviewmap.getTransforms().add(scale);

                    double xloc= HelloApplication.group.getLayoutX();
                    double yloc = HelloApplication.group.getLayoutY();

                    //System.out.println("Current X position "+xloc);
                    //System.out.println("Current Y position "+yloc);

                    //System.out.println("Current WX of map =" + stage.getWidth());
                    //System.out.println("Current WY of map =" + stage.getHeight() );

                    activeAreaRect =  new Rectangle(stage.getWidth()*HelloApplication.minimapScale,
                        stage.getHeight()*HelloApplication.minimapScale);
                    activeAreaRect.setFill(Color.TRANSPARENT);
                    activeAreaRect.setStrokeWidth(2);
                    activeAreaRect.setStroke(Color.YELLOW);

                    activeAreaRect.setX(HelloApplication.minimapBaseX+ (-1.0*xloc)*HelloApplication.minimapScale);
                    activeAreaRect.setY(HelloApplication.minimapBaseY+ (-1.0*yloc)*HelloApplication.minimapScale);
                    group2.getChildren().add(activeAreaRect);
                    //-------------------------------------------------------------------------
                }
                else frames++;




            }
        };
        timer.start();

        AddingCLI cli = new AddingCLI(world);
        Thread cliThread = new Thread(cli::processCommandLine);
        cliThread.setDaemon(true);
        cliThread.start();

        world.CLIinitialize();

    }
    public double getScreenWX(){
        return HelloApplication.stage.getWidth();
    }

    public double getScreenWY(){
        return HelloApplication.stage.getHeight();
    }
    public static void main(String[] args) {

        launch();
//        String str;
//        int count=0;
//
//        Scanner in = new Scanner(System.in);
//
//        do{
//            System.out.print("Enter string N"+(count+1)+":");
//            str= in.nextLine();
//            System.out.println(str);
//            count++;
//            if(count==3)launch();
//
//        }while(count<5);
//

    }
}
