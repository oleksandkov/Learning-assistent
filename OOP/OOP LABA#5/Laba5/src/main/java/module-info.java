module org.example.laba5 {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.web;

    opens org.example.laba5 to javafx.fxml;
    exports org.example.laba5;
}