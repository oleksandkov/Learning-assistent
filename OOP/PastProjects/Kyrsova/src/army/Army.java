package army;

import objects.micro.MusicianMaster;
import objects.micro.OrchestraConductor;
import objects.micro.Shopper;
import sample.Main;

public class Army {
    private static boolean isArmy = false;
    private static int N1, N2, N3;
    public static int N1Ready=0, N2Ready=0,N3Ready=0;
    public static int N1ActiveReady=0, N2ActiveReady=0,N3ActiveReady=0;
    final public static int RADIUS = 300;
    final public static int START_X = 150;



    public static void toArmy(double x, double y){
        for (Shopper shopper: Main.getCity().getShoppers()){
            if (shopper.getXChord()>=x-Army.RADIUS && shopper.getXChord()<=x+Army.RADIUS && shopper.getYChord()>=y-Army.RADIUS && shopper.getYChord()<=y+ Army.RADIUS){
                if (!shopper.isInArmy()){
                    if (shopper instanceof OrchestraConductor && N3Ready+ N3ActiveReady< N3){
                        if (shopper.isActive()) {
                            shopper.setNumberInRank(++N3ActiveReady);
                        } else {
                            shopper.setNumberInRank(++N3Ready);
                        }
                        shopper.setInArmy(true);
                    } else if (shopper instanceof MusicianMaster && N2Ready+N2ActiveReady<N2){
                        if (shopper.isActive()) {
                            shopper.setNumberInRank(++N2ActiveReady);
                        } else {
                            shopper.setNumberInRank(++N2Ready);
                        }
                        shopper.setInArmy(true);
                    } else if (N1Ready+ N1ActiveReady < N1) {
                        if (shopper.isActive()) {
                            shopper.setNumberInRank(++N1ActiveReady);
                        } else {
                            shopper.setNumberInRank(++N1Ready);
                        }
                        shopper.setInArmy(true);
                    }
                }
            }
        }
    }

    public static boolean checkArmyFinish(){
        if (Army.isArmy() && N1Ready+N1ActiveReady+N2Ready+N2ActiveReady+N3Ready+N3ActiveReady!=0) {
            for (Shopper shopper : Main.getCity().getShoppers()) {
                if (shopper.isInArmy()) {
                    if (!shopper.isxStay() || !shopper.isyStay()) {
                        return false;
                    }
                }
            }
        } else {
            return false;
        }
        return true;
    }

    public static boolean isArmy() {
        return isArmy;
    }

    public static void setArmy(boolean isArmy) {
        Army.isArmy = isArmy;
    }

    public static int getN1() {
        return N1;
    }

    public static void setN1(int n1) {
        N1 = n1;
    }

    public static int getN2() {
        return N2;
    }

    public static void setN2(int n2) {
        N2 = n2;
    }

    public static int getN3() {
        return N3;
    }

    public static void setN3(int n3) {
        N3 = n3;
    }
}
