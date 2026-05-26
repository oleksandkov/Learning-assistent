module org.example.lab4 {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.logging;


    opens org.example.lab4 to javafx.fxml;
    exports org.example.lab4;
}