package sample.windows.preferencesWindow;

import sample.Main;

public class Preferences {
    private static boolean MAP = true;
    private static boolean INFORMATION = true;
    private static double VOLUME = 0.4;
    private static double SPEED = 7;
    private static Complexity COMPLEXITY = Complexity.LIGHT;


    public static boolean isMAP() {
        return MAP;
    }

    public static boolean isINFORMATION() {
        return INFORMATION;
    }

    public static double getVOLUME() {
        return VOLUME;
    }

    public static double getSPEED() {
        return SPEED;
    }

    public static void setMAP(boolean MAP) {
        Preferences.MAP = MAP;
    }

    public static void setINFORMATION(boolean INFORMATION) {
        Preferences.INFORMATION = INFORMATION;
    }

    public static void setVOLUME(double VOLUME) {
        Preferences.VOLUME = VOLUME;
    }

    public static void setSPEED(double SPEED) {
        Preferences.SPEED = SPEED;
    }
    public static void setCOMPLEXITY(Complexity complexity)
    {
        COMPLEXITY = complexity;
    }

    public static Complexity getCOMPLEXITY() {
        return COMPLEXITY;
    }
}
