package org.example.laba5;

import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import org.example.laba5.Unit.Unit;
import org.example.laba5.Unit.Warrior;

public class UnitInventorWindow {
    private final Stage inventoryStage;
    private final Label inventoryTitleLabel;
    private final VBox inventoryItemsBox;
    private HBox oreCountBox;
    private Label oreCountField;
    private final Map<String, Image> inventoryIcons = new HashMap<>();

    public UnitInventorWindow() {
        initInventoryIcons();

        inventoryStage = new Stage();
        inventoryStage.setTitle("Unit Inventory");

        inventoryTitleLabel = new Label("No active unit");
        inventoryItemsBox = new VBox(6);

        VBox root = new VBox(10, inventoryTitleLabel, inventoryItemsBox);
        root.setPadding(new Insets(12));

        Scene inventoryScene = new Scene(root, 280, 280);
        inventoryStage.setScene(inventoryScene);
        inventoryStage.setX(1120);
        inventoryStage.setY(120);
        inventoryStage.hide();
    }

    public void updateFromUnits(List<Unit> units) {
        if (units == null) {
            return;
        }

        Unit activeUnit = units.stream().filter(u -> u != null && u.isActive()).findFirst().orElse(null);

        inventoryItemsBox.getChildren().clear();

        if (activeUnit == null) {
            inventoryTitleLabel.setText("No active unit");
            inventoryItemsBox.getChildren().add(new Label("Select a unit to view inventory"));
            if (isVisible()) {
                setVisible(false);
            }
            return;
        }

        inventoryTitleLabel.setText(activeUnit.getClass().getSimpleName() + " inventory");
        List<String> inventory = activeUnit.getInventor();
        if (inventory == null || inventory.isEmpty()) {
            inventoryItemsBox.getChildren().add(new Label("Inventory is empty"));
        } else {
            inventory.forEach(item -> {
                String itemName = item == null ? "Unknown" : item;
                String iconKey = itemName.trim().toLowerCase();
                Image icon = inventoryIcons.get(iconKey);

                HBox row = new HBox(8);
                if (icon != null) {
                    ImageView itemIcon = new ImageView(icon);
                    itemIcon.setFitWidth(28);
                    itemIcon.setFitHeight(28);
                    row.getChildren().add(itemIcon);
                }
                row.getChildren().add(new Label(itemName));
                inventoryItemsBox.getChildren().add(row);
            });
        }

        if (activeUnit.getClass().getSimpleName().equals("Warrior")) {
            Warrior warrior = (Warrior) activeUnit;
            
            if (oreCountBox == null) {
                oreCountBox = new HBox(10);
                oreCountBox.setPadding(new Insets(15, 0, 0, 0));
                Label oreCountLabel = new Label("Ore count:");
                oreCountLabel.setPrefWidth(80);
                oreCountField = new Label();
                oreCountBox.getChildren().addAll(oreCountLabel, oreCountField);
            }
            
            oreCountField.setText(String.valueOf((int) warrior.getOre()));
            inventoryItemsBox.getChildren().add(oreCountBox);
        }
    }

    public boolean showForActiveUnit(List<Unit> units) {
        if (!hasActiveUnit(units)) {
            setVisible(false);
            return false;
        }
        setVisible(true);
        return true;
    }

    public boolean hasActiveUnit(List<Unit> units) {
        if (units == null) {
            return false;
        }
        return units.stream().anyMatch(u -> u != null && u.isActive());
    }

    public boolean isVisible() {
        return inventoryStage != null && inventoryStage.isShowing();
    }

    public void setVisible(boolean visible) {
        if (inventoryStage == null) {
            return;
        }
        if (visible) {
            inventoryStage.show();
            inventoryStage.toFront();
        } else {
            inventoryStage.hide();
        }
    }

    private void initInventoryIcons() {
        inventoryIcons.clear();
        registerInventoryIcon("sword", "/sword.png");
        registerInventoryIcon("knife", "/knife.png");
        registerInventoryIcon("spear", "/spear.png");
        registerInventoryIcon("bow", "/bow.png");
    }

    private void registerInventoryIcon(String key, String resourcePath) {
        URL url = HelloApplication.class.getResource(resourcePath);
        if (url != null) {
            inventoryIcons.put(key, new Image(url.toExternalForm(), 28, 28, true, true));
        }
    }
}
