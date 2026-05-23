package sample;

import javafx.application.Platform;
import javafx.scene.control.*;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import sun.text.normalizer.UnicodeSet;

import java.io.*;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;

public class TopMenu {
    public static Stage stage;
    //File menu
    public static void SpawnMenu(){
        Menu fileMenu = new Menu("File");

        MenuItem openFile = new MenuItem("Open...");
        openFile.setOnAction(e -> deserialize());
        fileMenu.getItems().add(openFile);


        MenuItem saveFile = new MenuItem("Save...");
        saveFile.setOnAction(e -> serialize());
        fileMenu.getItems().add(saveFile);

        fileMenu.getItems().add(new SeparatorMenuItem());
        fileMenu.getItems().add(new MenuItem("Settings..."));
        fileMenu.getItems().add(new SeparatorMenuItem());

        MenuItem exitFile = new MenuItem("Exit...");
        exitFile.setOnAction(e -> Platform.exit()); // fileMenu.hide()
        fileMenu.getItems().add(exitFile);

        //Edit menu
        Menu editMenu = new Menu("_Edit");

        MenuItem cloneFile = new MenuItem("Clone...");
        cloneFile.setOnAction(e -> {
            try {
                cloneMar();
            } catch (CloneNotSupportedException ex) {
                ex.printStackTrace();
            }
        });
        editMenu.getItems().add(cloneFile);

        MenuItem info = new MenuItem("Info...");
        info.setOnAction(e -> info());
        editMenu.getItems().add(info);


        //Help menu
        Menu helpMenu = new Menu("Help");
        CheckMenuItem showLines = new CheckMenuItem("Show Line Numbers");
        showLines.setOnAction(e -> {
            if (showLines.isSelected())
                System.out.println("Program will now display line numbers");
            else
                System.out.println("Hiding line number");
        });
        CheckMenuItem autoSave = new CheckMenuItem("Enable Autosave");
        autoSave.setSelected(true);
        helpMenu.getItems().addAll(showLines, autoSave);

/*
        editMenu.getItems().add(new MenuItem("Cut"));
        editMenu.getItems().add(new MenuItem("Copy"));
        MenuItem paste = new MenuItem("Paste");
        paste.setOnAction(e -> System.out.println("Pasting some crap"));
        paste.setDisable(true);
        editMenu.getItems().add(paste);

        //Help menu
        Menu helpMenu = new Menu("Help");
        CheckMenuItem showLines = new CheckMenuItem("Show Line Numbers");
        showLines.setOnAction(e -> {
            if (showLines.isSelected())
                System.out.println("Program will now display line numbers");
            else
                System.out.println("Hiding line number");
        });
        CheckMenuItem autoSave = new CheckMenuItem("Enable Autosave");
        autoSave.setSelected(true);
        helpMenu.getItems().addAll(showLines, autoSave);

        //Difficulty RadioMenuItems
        Menu difficultyMenu = new Menu("Difficulty");
        ToggleGroup difficultyToggle = new ToggleGroup();

        RadioMenuItem easy = new RadioMenuItem("Easy");
        RadioMenuItem medium = new RadioMenuItem("Medium");
        RadioMenuItem hard = new RadioMenuItem("Hard");

        easy.setToggleGroup(difficultyToggle);
        medium.setToggleGroup(difficultyToggle);
        hard.setToggleGroup(difficultyToggle);

        difficultyMenu.getItems().addAll(easy, medium, hard);
*/
        //Main menu bar
        MenuBar menuBar = new MenuBar();
  //      menuBar.getMenus().addAll(fileMenu, editMenu, helpMenu, difficultyMenu);
        menuBar.getMenus().addAll(fileMenu,editMenu,helpMenu);
        Main.layout.setTop(menuBar);
    }

    private static void deserialize() {
        String currentDir= System.getProperty("user.dir");
        File initDirectory = new File(currentDir);
        FileChooser fileChooser = new FileChooser();
        FileChooser.ExtensionFilter extFilter =
                new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
        fileChooser.getExtensionFilters().add(extFilter);
        fileChooser.setInitialDirectory(
                initDirectory);
        File file = fileChooser.showOpenDialog(stage);
        if(file != null) {
            BufferedReader bufferedReader = null;
            try {
                bufferedReader = new BufferedReader(new FileReader(file));
                String text;
                text = bufferedReader.readLine();
                int hmany = Integer.parseInt(text);
                for (Pilz r : Main.pl) {Main.group.getChildren().remove(r.g); }
                Pilz.counter =0;
                Main.pl = new Pilz[hmany];
                for (int i = 0; i < hmany; i++) {
                    Main.pl[i] = new Pilz();
                    Main.pl[i].Open(bufferedReader);
                    Main.group.getChildren().add(Main.pl[i].g);
                }
                //--------------------------------------
                text = bufferedReader.readLine();
                hmany = Integer.parseInt(text);
                for (Mario r : Main.mr) {
                    Main.groupMar.getChildren().remove(r.getGroup());
                }
                Main.mr = new Mario[hmany];
                int type;
                for (int i = 0; i < hmany; i++) {
                    text = bufferedReader.readLine();
                    type = Integer.parseInt(text);

                    if (type == 1) {
                        Main.mr[i] = new Mario();
                        Main.mr[i].Open(bufferedReader);
                    } else if (type == 2) {
                        Main.mr[i] = new Drago();
                        Main.mr[i].Open(bufferedReader);
                    } else if (type == 3) {
                        Main.mr[i] = new Princess();
                        Main.mr[i].Open(bufferedReader);
                    }
                    Main.groupMar.getChildren().add(Main.mr[i].getGroup());
                }
                //--------------------------------------
            } catch (FileNotFoundException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
            } finally {
                try {
                    bufferedReader.close();
                } catch (IOException ex) {
                    Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
    }
    private static void serialize() {
        String currentDir = System.getProperty("user.dir");

        File initDirectory = new File(currentDir);

        FileChooser fileChooser = new FileChooser();

        FileChooser.ExtensionFilter extFilter =
                new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
        fileChooser.getExtensionFilters().add(extFilter);
        fileChooser.setInitialDirectory(
                initDirectory);
        //Show save file dialog
        File file = fileChooser.showSaveDialog(stage);

        if(file != null) {

            try {
                FileWriter fileWriter = null;
                fileWriter = new FileWriter(file);

                fileWriter.write(Integer.toString(Main.pl.length));
                fileWriter.write("\n");
                for( Pilz r:Main.pl )
                {
                    r.Save(fileWriter);
                }

                fileWriter.write(Integer.toString(Main.mr.length));
                fileWriter.write("\n");

                for( Mario r:Main.mr )
                {
                    r.Save(fileWriter);
                }
                fileWriter.close();
            } catch (IOException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public static void cloneMar() throws CloneNotSupportedException {
        Main.timer.stop();
        Mario[] tmparray; // = java.util.Arrays.copyOf(Main.mr, Main.mr.length);
        Mario[] array = Main.mr;
        for (int i = 0; i < Main.mr.length; i++) {
            if (Main.mr[i].isActive()) {
                tmparray = java.util.Arrays.copyOf(
                        Main.mr, Main.mr.length + 1);

                tmparray[Main.mr.length] = Main.mr[i].clone();
                Main.mr = tmparray;
                System.out.println("Cloned, " +
                        "but it is impossiblle to clone() the graphical part.");

            }
        }
        if(array.length < Main.mr.length) {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Cloning process");
            alert.setHeaderText("Results:");
            alert.setContentText("Clonning was successful!");

            alert.showAndWait();
        }

        Main.timer.start();
    }

    static String genStat() {
        System.out.println("[General info]");
        String s = Arrays.toString(Main.mr);				//???
        return s;
    }

    private static void info() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Info");
        alert.setHeaderText("Here's arrrray:");
        alert.setContentText(genStat());
        alert.showAndWait();

    }


}
