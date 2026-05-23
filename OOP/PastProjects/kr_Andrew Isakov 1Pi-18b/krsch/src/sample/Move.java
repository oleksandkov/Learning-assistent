package sample;

import javafx.beans.property.ReadOnlyObjectProperty;
import javafx.geometry.Bounds;

public class Move {
    private double speed;

     public void setSpeed(double speed) {
        this.speed = speed;
    }
public void catchMove(){

}

// Nonstatic Move() backup
/*
  public void Move() {
        //rnd_move =true;

        if (isActive()) return;
        if ( !(to_Base && rnd_move)) {
//        if ((g.getLayoutX() == newx) &&
//                (g.getLayoutY() == newy)) {
            // find the closest Pilz
            Point2D p2d = getNextMoveToPilz(g.getLayoutX(), g.getLayoutY());
            Bounds bounds = g.getLayoutBounds();
            //System.out.println(bounds);
            Point2D coordinates = g.localToScene(bounds.getMinX(), bounds.getMinY());
//            int X = (int) coordinates.getX();
//            int Y = (int) coordinates.getY();
//            System.out.println(X);
//            System.out.println(Y);
            //+iwolf.boundsInParentProperty().get().getWidth()/2.0,
            //+iwolf.boundsInParentProperty().get().getHeight()/2.0

            newx = p2d.getX();
            newy = p2d.getY();
               // touchOther(Main.mr);
            eatOne(Main.mr);
        //  newx and newy differently
        //     double dx = newx - g.getLayoutX();
        //    double dy = newy - g.getLayoutY();

        // }

        double dx = newx - g.getLayoutX();
        dx = (Math.abs(dx) > maxdx) ? Math.signum(dx) * maxdx : dx;
        double dy = newy - g.getLayoutY();
        dy = (Math.abs(dy) > maxdy) ? Math.signum(dy) * maxdy : dy;

        g.setLayoutX(g.getLayoutX() + dx);
        g.setLayoutY(g.getLayoutY() + dy);
    }
        // some random movement
        else if(rnd_move) {
        //Point2D point = newRANDpntCreator();
        int x = (int) g.getLayoutX();
        System.out.println((int) g.getLayoutX());
        int y = (int) g.getLayoutY();
        if (x == point.getX() && y == point.getY()) {
            point = newRANDpntCreator();
        }
        int go = 1;
        if (x > point.getX()) {
            g.setLayoutX(g.getLayoutX() - step);
        } else if (y > point.getY()) {
            g.setLayoutY(g.getLayoutY() - step);
        } else if (x < point.getX()) {
            g.setLayoutX(g.getLayoutX() + step);
        } else if (y < point.getY()) {
            g.setLayoutY(g.getLayoutY() + step);
        }
    }


}
*/


/*
    public void touchOther(Mario [] mr){
        for (int i = 0; i < mr.length; i++) {
            if(mr[i] != null)  {
                eat(mr,

                if ((Main.mr[i].ivMar.getX() ==
                        Main.mr[j].getGroup().
                (Main.mr[i].getGroup().getTranslateY() ==
                        Main.mr[j].getGroup().getTranslateY()))
                Main.mr[i].switchActivation();
                //Main.mr[j].switchActivation();
                if (Main.mr[i].getSide() != Main.mr[j].getSide()) {
                    Main.mr[i].Damage();
                    Main.mr[j].Damage();
                }
                newx = Math.abs(Main.mr[j].getGroup().getLayoutX() - Main.mr[i].getGroup().getLayoutX()
                        + Main.mr[i].getGroup().getLayoutBounds().getWidth());
                newy = Math.abs(Main.mr[j].getGroup().getLayoutY() - Main.mr[i].getGroup().getLayoutY()
                        + Main.mr[i].getGroup().getLayoutBounds().getHeight());
                System.out.println("Damagetest" + newx);
                System.out.println("Damagetest" + newy);
            }
        }
    }

*/






// something
    /*
    if ( !(m.health <= 1)) {
        System.out.println("Delete this bastard/////");
        System.out.println("Main mr lengtdsagh " + Main.mr.length);
        Mario tmpr;
        for (int count = 0; count < tmp.length; count++) {
            //for (Mario tmpr : Main.mr) {
            tmpr = Main.mr[count];

            if (tmpr != this) {
                tmp[count] = tmpr;

                System.out.println("Yes..." + Main.mr.length);
            }
        }

     */





}
