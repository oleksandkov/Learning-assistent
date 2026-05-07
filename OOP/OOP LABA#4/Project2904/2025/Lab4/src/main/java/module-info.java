module org.example.lab4 {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.graphics;


    opens org.example.lab4 to javafx.fxml;
    exports org.example.lab4;
}