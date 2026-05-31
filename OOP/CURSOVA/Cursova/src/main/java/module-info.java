module org.example.laba5 {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.xml;

    opens org.example.laba5 to javafx.fxml;
    opens org.example.laba5.Unit to javafx.fxml;
    exports org.example.laba5;
    exports org.example.laba5.Unit;
}