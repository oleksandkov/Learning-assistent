module org.example.laba6 {
    requires javafx.controls;
    requires javafx.fxml;

    opens org.example.laba5 to javafx.fxml;
    exports org.example.laba5;
}