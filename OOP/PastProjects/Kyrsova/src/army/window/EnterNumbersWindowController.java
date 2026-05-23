package army.window;

import army.Army;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;

public class EnterNumbersWindowController {
    @FXML
    private TextField textN1;

    @FXML
    private TextField textN2;

    @FXML
    private TextField textN3;

    @FXML
    private Button enterButton;

    @FXML
    void initialize() {
        enterButton.setOnAction(event -> {
            if (!textN1.getText().isEmpty() && !textN2.getText().isEmpty() && !textN3.getText().isEmpty()) {
                Army.setArmy(true);
                Army.setN1(Integer.parseInt(textN1.getText()));
                Army.setN2(Integer.parseInt(textN2.getText()));
                Army.setN3(Integer.parseInt(textN3.getText()));
                EnterNumbersWindow.getWindow().close();
            }
        });
    }
}
