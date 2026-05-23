package sample;
import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.stage.Stage;

import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.Random;
public class Main extends Application {
    public static Wallpaper wp;
    public static Pilz[] pl;
    public static BadBase badBase;
    public static GoodBase goodBase;
    public static Mario[] mr;
    public static Group groupMar = new Group();

static AnimationTimer timer ;
    public static Random rnd = new Random(); // for spawning Pilzen
    static Group group = new Group();
    static Scene scene;
    static BorderPane layout;

    public static ScrollPane scrollPane;

    static Label mars = new Label();
       static Label pilzs = new Label();
        //Tafel = new Group();                                    // 'board' in German
      static  int t,f,u,b,g; // some counters

    public void SpawnWallpaper() throws FileNotFoundException {
        wp = new Wallpaper();
        group.getChildren().add(wp.getWallGrp());
    };

    static void SpawnPilz(int size) throws FileNotFoundException {
        /// makroobj Pilz
        pl = new Pilz[size];
        double px, py;                      // spawn Marss
        int i = 0;
        outer:
        while (i < pl.length) {

            int max = (int) Wallpaper.iv.getFitWidth();
            px = rnd.nextInt(max - 300);

            max = (int) Wallpaper.iv.getFitHeight();
            py = rnd.nextInt(max - 300);

// I don't want my mushrooms to intersect
            for (int j = 0; j < pl.length; j++) {
                if ((pl[j] != null) &&
                        pl[j].getGroup_Pilz().getBoundsInParent().intersects(px,py,300,300)
               || Main.goodBase.g.getBoundsInParent().intersects(px, py, 300, 300)
                        || Main.badBase.g.getBoundsInParent().intersects(px, py, 300, 300)
                ) {
                    continue outer; // goes to while and begins it once again read about goto in Java
                    // https://stackoverflow.com/questions/2545103/is-there-a-goto-statement-in-java
                }
            }
            pl[i] = new Pilz(px, py);
            i++;
        }
        for (Pilz m : pl)
        {
            group.getChildren().add(m.g);
        }
    }

    public void SpawnMars (int size) throws FileNotFoundException {
        mr = new Mario[size];
        for (int i = 0; i < mr.length; i++){
            mr[i] = new Mario();
        }

        for (Mario m : mr) {
            groupMar.getChildren().add(m.getGroup());
        }

    }

    public static void Show_teams(){

        t=f=u=b=g=0;
        for(int i = 0; i<Main.mr.length; i++){
            if(Main.mr[i].getSide() == true) g++;
            if(Main.mr[i].getSide() == false) b++;
        }
       // mars.setText("") ;
        mars.setText("Heroes: |Good "+g+" |Bad "+b+" |");
        for(int i = 0; i<pl.length; i++){
            if(Main.pl[i].getSide() == 1) t++;
            if(Main.pl[i].getSide() == -1) f++;
            if(Main.pl[i].getSide() == 0) u++;

        }
        //pilzs.setText(null) ;
        //pilzs.setText(" ") ;

        pilzs.setText("Pilzes : |Good "+t+" |Bad "+f+" | Undefined: "+u+" |");
        pilzs.setFont(new Font("Arial", 20));
        mars.setFont(new Font("Arial", 20));

            if(u == 0){
                timer.stop();
                if(t>f) {
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("Congratulations!");
                    alert.setHeaderText(null);
                    // alert.setHeaderText("Results:");
                    alert.setContentText("The forces of Good had won successfully!!!");
                    Platform.runLater(alert::showAndWait);
                   alert.setOnCloseRequest(e -> Platform.exit());
                }
                if(t<f) {
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("Congratulations!");

                    alert.setHeaderText(null);
                    // alert.setHeaderText("Results:");
                    alert.setContentText("Alas, the forces of Bad had won...");

                    //alert.showAndWait();
                    Platform.runLater(alert::showAndWait);
                    alert.setOnCloseRequest(e -> Platform.exit());
                }
                    if(t==f) {
                     Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("Congratulations!");

                    alert.setHeaderText(null);
                    // alert.setHeaderText("Results:");
                    alert.setContentText("Draw");

                    //alert.showAndWait();
                    Platform.runLater(alert::showAndWait);
                    alert.setOnCloseRequest(e -> Platform.exit());
                }
            }
           /* else if(g==0){
                timer.stop();

                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("Congratulations!");

                alert.setHeaderText(null);
                    // alert.setHeaderText("Results:");
                    alert.setContentText("Alas, the forces of Bad defeated Good...");

//                    alert.showAndWait();
               Platform.runLater(alert::showAndWait);
                alert.setOnCloseRequest(e -> Platform.exit());
            }
             else if(b==0){
                timer.stop();
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("Congratulations!");

                alert.setHeaderText(null);
                    // alert.setHeaderText("Results:");
                    alert.setContentText("The forces of Good defeated all the Bad...");

               //     alert.showAndWait();
                Platform.runLater(alert::showAndWait);
                alert.setOnCloseRequest(e -> Platform.exit());
            }

            */
            }

    @Override
    public void start(Stage stage) throws Exception {
        SpawnWallpaper();

        badBase = new BadBase(Wallpaper.border.getWidth()-800, 100);
        group.getChildren().add(badBase.g);

        goodBase = new GoodBase(500, 100);
        group.getChildren().add(goodBase.g);

        SpawnPilz(10);
        SpawnMars(10);
        Group root = new Group(group, groupMar);
        //Group topLayerForTafel = new Group(root, Tafel);

        //Tafel.relocate(300,0);
        scrollPane = new ScrollPane(root);

        scrollPane.setMaxWidth(Wallpaper.border.getWidth());//Wallpaper.iv.getFitWidth()); 4600
        scrollPane.setMaxHeight(Wallpaper.border.getHeight());//Wallpaper.iv.getFitWidth());
        // pane.setMinHeight(4000); //Wallpaper.iv.getFitHeight());

        scrollPane.setFitToHeight(true);
        scrollPane.setFitToWidth(true);
        //
        layout = new BorderPane();

        TopMenu.SpawnMenu(); // adds it to the top of layout
        layout.setCenter(scrollPane);


        scene = new Scene(layout, 600,700);

        // Magic here
        scene.setOnKeyPressed(new KeyPressedHandler());

        timer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                lifeCycle();
            }
        };

        // Check who won
        t=f=u=b=g=0;
        for(int i = 0; i<Main.mr.length; i++){
            if(Main.mr[i].getSide() == true) g++;
            if(Main.mr[i].getSide() == false) b++;
        }
        // mars.setText("") ;
        // mars.setText("Heroes: |Good "+g+" |Bad "+b+" |");
        for(int i = 0; i<pl.length; i++){
            if(Main.pl[i].getSide() == 1) t++;
            if(Main.pl[i].getSide() == -1) f++;
            if(Main.pl[i].getSide() == 0) u++;
        }

        Wallpaper.Tafel.getChildren().addAll(mars,pilzs);
        pilzs.relocate(0, 23);
        stage.setTitle("Mario Game");
        stage.setScene(scene);
        timer.start();
        stage.show();
    }

    public void lifeCycle(){
        // it's a very bad idea to put static functions inside nonstatic
        // especially if the static f change the array that is used to
        // loop the nonstatic function
        boolean b = Mario.isHealthy(Main.mr);
        Main.mr = Mario.DeleteNull(Main.mr,b);       // Clean up dying things
        Mario.eatOne(Main.mr);
        for(int i =0; i< Main.mr.length;i++) {
            mr[i].Move();
        }
        Mario.touchcheck(Main.mr, Main.pl);
        Show_teams();
    }

    private class KeyPressedHandler implements EventHandler<KeyEvent> {
        @Override
        public void handle(KeyEvent event) {
            // Actions
            if (event.getCode().equals(KeyCode.INSERT)) {
                Dial.dial();
                if (Dial.c == 0) ;
                else if (Dial.c == 1) {
                    try {
                        Main.mr = Arrays.copyOf(Main.mr, Main.mr.length + 1);
                        Mario m = new Mario(Dial.active,
                                Dial.side,
                                Dial.x, Dial.y,
                                Dial.speed);
                        Main.mr[Main.mr.length - 1] = m;

                        groupMar.getChildren().add(Main.mr[Main.mr.length - 1].g);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }
                // Bad guy... Luigi? or some sort of tortoise?
              /*  else if (Dial.c == 4) {
                    try {
                        Main.mr = Arrays.copyOf(Main.mr, Main.mr.length + 1);
                        Mario m = new Princess(Dial.active,    // Mario m = new Princess
                                // Dynamic polymorphism can be prodused this way by overriding a parent(Mario) method in child class(Princess)
                                Dial.side,
                                Dial.x, Dial.y,
                                Dial.speed);
                        Main.mr[Main.mr.length - 1] = m;

                        group.getChildren().add(Main.mr[Main.mr.length - 1].g);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }
                // Bad .end

               */
                else if (Dial.c == 3) {
                    try {
                        Main.mr = Arrays.copyOf(Main.mr, Main.mr.length + 1);
                        Mario m = new Princess(Dial.active,
                                Dial.side,
                                Dial.x, Dial.y,
                                Dial.speed);
                        Main.mr[Main.mr.length - 1] = m;

                        group.getChildren().add(Main.mr[Main.mr.length - 1].g);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }
                else if (Dial.c == 2) {
                    try {
                        Main.mr = Arrays.copyOf(Main.mr, Main.mr.length + 1);
                        Mario m = new Drago(Dial.active,
                                Dial.side,
                                Dial.x, Dial.y,
                                Dial.speed);
                        Main.mr[Main.mr.length - 1] = m;

                        group.getChildren().add(Main.mr[Main.mr.length - 1].g);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }
                Dial.active = false;
                Dial.c=0;
                //Dial.side = false;
            }

            if (event.getCode().equals(KeyCode.DELETE)) {

                int count=0;
                for( Mario r :mr)
                {
                    if(r.active) {
                        groupMar.getChildren().remove(r.g);
                        r.g.setVisible(false);              // this will be great if it won't show up when sort (ing) array or cloning
                        count++;
                    }

                }

                Mario [] tmp=new Mario[mr.length-count];

                count=0;
                for( Mario r :mr)
                {
                    if(!r.active) {
                        tmp[count]=r;
                        count++;
                    }

                }
                mr= tmp;

            }

            if (event.getCode().equals(KeyCode.ESCAPE)) {
                for(int i = 0; i< Main.mr.length; i++)
                {
                    if(Main.mr[i].active == true) {
                        Main.mr[i].active= !Main.mr[i].active;

                        if (Main.mr[i].active)
                            Main.mr[i].r.setFill(Color.RED);
                        else
                            Main.mr[i].r.setFill(Color.YELLOW);
                    }
                }
            }
            if (event.getCode().equals(KeyCode.R)) {
                Mario.SwitchRnd_Move();
            }
            if (event.getCode().equals(KeyCode.B)) {
                Mario.SwitchTo_Base();
            }

            if (event.getCode().equals(KeyCode.C)) {
                Mario.compar();
            }
            // Movements
            if (event.getCode().equals(KeyCode.UP)) {
                for (Mario m : mr) {
                    m.Up();
                }
            }

            if (event.getCode().equals(KeyCode.RIGHT)) {
                for (Mario m : mr) {
                    m.Right();
                }
            }

            if (event.getCode().equals(KeyCode.DOWN)) {
                for (Mario m : mr) {
                    m.Down();
                }
            }

            if (event.getCode().equals(KeyCode.LEFT)) {
                for (Mario m : mr) {
                    m.Left();
                }
            }
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
