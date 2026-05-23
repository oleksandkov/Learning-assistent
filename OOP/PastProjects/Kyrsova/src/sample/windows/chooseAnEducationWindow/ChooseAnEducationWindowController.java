package sample.windows.chooseAnEducationWindow;


import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import objects.micro.Shopper;
import sample.Main;
import sample.windows.preferencesWindow.Preferences;

public class ChooseAnEducationWindowController {

    private boolean firstEd = false;
    private boolean secondEd = false;

    private double allPr = 0;

    @FXML
    private Button chooseButton;

    @FXML
    private CheckBox firstEducation;

    @FXML
    private CheckBox secondEducation;

    @FXML
    private Label allPrice;

    @FXML
    void initialize() {
        double FIRST_PRICE = Preferences.getCOMPLEXITY().getFirstEducationPrice();
        double SECOND_PRICE = Preferences.getCOMPLEXITY().getSecondEducationPrice();
        allPrice.setText(Double.toString(0));
        firstEducation.setOnAction(event -> {
            firstEd = !firstEd;
            if (firstEd && secondEd)
                allPr = FIRST_PRICE+SECOND_PRICE;
            else if (firstEd)
                allPr = FIRST_PRICE;
            else if (secondEd)
                allPr = SECOND_PRICE;
            else allPr = 0;
            allPrice.setText(Double.toString(allPr));
        });
        secondEducation.setOnAction(event -> {
            secondEd = !secondEd;
            if (firstEd && secondEd)
                allPr = FIRST_PRICE+SECOND_PRICE;
            else if (firstEd)
                allPr = FIRST_PRICE;
            else if (secondEd)
                allPr = SECOND_PRICE;
            else allPr = 0;
            allPrice.setText(Double.toString(allPr));
        });
        chooseButton.setOnAction(event -> {
            if (firstEd || secondEd) {
                Shopper old = ChooseAnEducationWindow.getShopper();
                Shopper temp = old.education(firstEd, secondEd, allPr);
                if (old != temp){
                    temp.setYChord(old.getYChord());
                    temp.setXChord(old.getXChord());
                    Main.getCity().deleteAShopper(old);
                    Main.getCity().addNewShopper(temp,false);
                }

                ChooseAnEducationWindow.getWindow().close();
            }
        });
    }

}