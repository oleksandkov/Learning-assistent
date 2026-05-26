package org.example.lab4;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;

public class DrugijCorpus extends Location {
    public void load(BufferedReader bufferedReader) throws IOException {
        super.load(bufferedReader);

        String text;
        text = bufferedReader.readLine();
        int i1= Integer.parseInt(text);
        time_learn= i1;

        text = bufferedReader.readLine();
        int i2= Integer.parseInt(text);
        time_learn_max= i2;

    }
    public void save(FileWriter fileWriter ) throws IOException {
        super.save(fileWriter);

        fileWriter.write(Integer.toString(time_learn)+"\n");
        fileWriter.write(Integer.toString(time_learn_max)+"\n");
    }

    private int time_learn;
    private int time_learn_max;

    public DrugijCorpus(Image img, double x, double y){
        super(10.0, 10.0, 80.0, 80.0, 24, 6 );

        image= new ImageView(img);
        HelloApplication.group.getChildren().add(image);

        image.setX(x);
        image.setY(y);

        time_delay=700;
        time_current=0;

        time_learn_max=15;
        time_learn=0;

        postInitialization();

        setString(World.askWorldNextCourse());
    }

    @Override
    public double studyPower(){
        return 2.0;
    }

    public void lifeCycle() {
        ++time_current;

        if(time_current<time_delay){
            ++time_learn;
            if( time_learn<time_learn_max )return;

            time_learn=0;
            String courseName= getString();

            for( int i=0; i<places.length; ++i ){
               if( places[i]!=null )
                   places[i].study(courseName, studyPower() );
            }

            return;
        }
        else {
            time_current = 0;

            setString(World.askWorldNextCourse());
        }
    }

}

