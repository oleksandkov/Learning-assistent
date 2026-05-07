module org.example.lab4_1 {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.example.lab4_1 to javafx.fxml;
    exports org.example.lab4_1;
}