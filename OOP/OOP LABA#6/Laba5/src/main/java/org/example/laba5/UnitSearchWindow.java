package org.example.laba5;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.collections.FXCollections;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.util.Duration;

public class UnitSearchWindow {
	private final Stage stage;
	private final ComboBox<String> classCombo;
	private final ComboBox<String> teamCombo;
	private final TextField healthField;
	private final TextField damageField;
	private final TextField inventorField;
	private final TextField macroField;
	private final ComboBox<String> sortCombo;
	private final ComboBox<String> countParameterCombo;
	private final ListView<Unit> resultsList;
	private final Label statusLabel;
	private final Label microobjectCountLabel;
	private final Timeline refreshTimer;
	private final World sortHelper = new World();

	public UnitSearchWindow() {
		stage = new Stage();
		stage.setTitle("Unit Search");

		classCombo = new ComboBox<>();
		classCombo.getItems().addAll("Any", "Warrior", "Centurio", "Pretorio");
		classCombo.setValue("Any");

		teamCombo = new ComboBox<>();
		teamCombo.getItems().addAll("Any", "Ally", "Enemy");
		teamCombo.setValue("Any");

		healthField = new TextField();
		healthField.setPromptText("Health");
		healthField.setTextFormatter(new TextFormatter<>(c -> c.getControlNewText().matches("\\d*") ? c : null));

		damageField = new TextField();
		damageField.setPromptText("Damage");
		damageField.setTextFormatter(new TextFormatter<>(c -> c.getControlNewText().matches("\\d*") ? c : null));

		inventorField = new TextField();
		inventorField.setPromptText("Inventory (comma separated)");

		macroField = new TextField();
		macroField.setPromptText("Macroobject name (or none)");

		sortCombo = new ComboBox<>();
		sortCombo.getItems().addAll("Class", "Health", "Damage");
		sortCombo.setValue("Class");

		countParameterCombo = new ComboBox<>();
		countParameterCombo.getItems().addAll("moreThanHalf", "haveSword", "team (ally)");
		countParameterCombo.setValue("moreThanHalf");

		HBox filtersRow1 = new HBox(8, new Label("Class:"), classCombo, new Label("Team:"), teamCombo);
		HBox filtersRow2 = new HBox(8, new Label("Health:"), healthField, new Label("Damage:"), damageField);
		HBox filtersRow3 = new HBox(8, new Label("Inventory:"), inventorField, new Label("Macro:"), macroField,
				new Label("Sort:"), sortCombo);
		Button removeAllButton = new Button("Remove all");
		removeAllButton.setOnAction(e -> removeAllFiltered());
		Button countMicroobjectsButton = new Button("Count microobjects");
		countMicroobjectsButton.setOnAction(e -> countMicroobjectsBySelectedParameter());

		resultsList = new ListView<>();
		resultsList.setCellFactory(list -> new ListCell<>() {
			@Override
			protected void updateItem(Unit unit, boolean empty) {
				super.updateItem(unit, empty);
				if (empty || unit == null) {
					setText(null);
					return;
				}
				setText(buildUnitDisplay(unit));
			}
		});

		resultsList.setOnMouseClicked(event -> {
			if (event.getClickCount() < 1) {
				return;
			}
			Unit selected = resultsList.getSelectionModel().getSelectedItem();
			if (selected != null) {
				if (!selected.isActive()) {
					selected.flipActivation();
				}
				setVisible(false);
			}
		});

		statusLabel = new Label("Matches: 0");
		microobjectCountLabel = new Label("Microobjects count: 0");

		HBox bottomRow = new HBox(8,
				new Label("Parameter:"),
				countParameterCombo,
				countMicroobjectsButton,
				microobjectCountLabel);

		VBox root = new VBox(10, filtersRow1, filtersRow2, filtersRow3, removeAllButton, statusLabel, resultsList, bottomRow);
		root.setPadding(new Insets(12));

		Scene scene = new Scene(root, 720, 420);
		stage.setScene(scene);

		classCombo.valueProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		teamCombo.valueProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		healthField.textProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		damageField.textProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		inventorField.textProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		macroField.textProperty().addListener((obs, oldVal, newVal) -> refreshList(true));
		sortCombo.valueProperty().addListener((obs, oldVal, newVal) -> refreshList(true));

		refreshTimer = new Timeline(new KeyFrame(Duration.millis(300), e -> refreshList(false)));
		refreshTimer.setCycleCount(Timeline.INDEFINITE);

		stage.setOnShown(e -> {
			refreshList(true);
			refreshTimer.play();
		});
		stage.setOnHidden(e -> refreshTimer.stop());
	}

	public void show() {
		stage.show();
		stage.toFront();
	}

	public boolean isVisible() {
		return stage.isShowing();
	}

	public void setVisible(boolean visible) {
		if (visible) {
			show();
		} else {
			stage.hide();
		}
	}

	private void refreshList(boolean resetSelection) {
		Unit selected = resetSelection ? null : resultsList.getSelectionModel().getSelectedItem();
		ArrayList<Unit> filtered = filterUnits();
		sortFilteredUnits(filtered);

		resultsList.setItems(FXCollections.observableArrayList(filtered));
		statusLabel.setText("Matches: " + filtered.size());

		if (!resetSelection && selected != null) {
			for (Unit unit : filtered) {
				if (unit == selected) {
					resultsList.getSelectionModel().select(unit);
					break;
				}
			}
		}
	}

	private ArrayList<Unit> filterUnits() {
		ArrayList<Unit> matches = new ArrayList<>();
		if (HelloApplication.units == null) {
			return matches;
		}

		List<Unit> snapshot = new ArrayList<>(HelloApplication.units);
		String classFilter = classCombo.getValue();
		String teamFilter = teamCombo.getValue();
		Integer healthFilter = parseIntOrNull(healthField.getText());
		Integer damageFilter = parseIntOrNull(damageField.getText());
		List<String> inventorFilter = parseInventory(inventorField.getText());
		String macroFilter = normalizeMacroFilter(macroField.getText());

		for (Unit unit : snapshot) {
			if (unit == null || Boolean.TRUE.equals(unit.getDead())) {
				continue;
			}

			if (!"Any".equals(classFilter) && !unit.getClass().getSimpleName().equals(classFilter)) {
				continue;
			}

			if (!"Any".equals(teamFilter)) {
				boolean isAlly = "Ally".equals(teamFilter);
				if (unit.getTeam() != isAlly) {
					continue;
				}
			}

			if (healthFilter != null) {
				Integer unitHealth = unit.getHealth();
				if (unitHealth == null || !unitHealth.equals(healthFilter)) {
					continue;
				}
			}

			if (damageFilter != null) {
				Integer unitDamage = unit.getDamage();
				if (unitDamage == null || !unitDamage.equals(damageFilter)) {
					continue;
				}
			}

			if (!inventorFilter.isEmpty()) {
				ArrayList<String> inv = unit.getInventor();
				if (inv == null) {
					continue;
				}
				List<String> invNormalized = new ArrayList<>();
				for (String item : inv) {
					if (item != null) {
						invNormalized.add(item.trim().toLowerCase(Locale.ROOT));
					}
				}
				boolean allPresent = true;
				for (String filterItem : inventorFilter) {
					if (!invNormalized.contains(filterItem)) {
						allPresent = false;
						break;
					}
				}
				if (!allPresent) {
					continue;
				}
			}

			if (macroFilter != null) {
				if (isNoneFilter(macroFilter)) {
					if (hasMacroMembership(unit)) {
						continue;
					}
				} else if (!isUnitInMacro(unit, macroFilter)) {
					continue;
				}
			}

			matches.add(unit);
		}

		return matches;
	}

	private String buildUnitDisplay(Unit unit) {
		String className = unit.getClass().getSimpleName();
		Integer health = unit.getHealth();
		Integer damage = unit.getDamage();
		String team = unit.getTeam() ? "Ally" : "Enemy";
		String inventor = unit.getInventor() == null ? "[]" : unit.getInventor().toString();

		String macros = buildMacroMembership(unit);

		return className
				+ " | health=" + (health == null ? "null" : health)
				+ " | damage=" + (damage == null ? "null" : damage)
				+ " | team=" + team
				+ " | inventory=" + inventor
			+ " | x=" + Math.round(unit.x)
			+ " | y=" + Math.round(unit.y)
				+ " | in=" + macros;
	}

	private String buildMacroMembership(Unit unit) {
		if (HelloApplication.buldings == null || HelloApplication.buldings.isEmpty()) {
			return "none";
		}

		ArrayList<String> memberships = new ArrayList<>();
		for (World world : HelloApplication.buldings) {
			if (world == null) {
				continue;
			}
			if (world.isUnitInside(unit)) {
				String team = world.getTeam() ? "Ally" : "Enemy";
				String macroName = (world.name == null || world.name.isBlank())
						? world.getClass().getSimpleName()
						: world.name;
				memberships.add(macroName + "(" + team + ")");
			}
		}

		if (memberships.isEmpty()) {
			return "none";
		}
		return String.join(", ", memberships);
	}

	private String normalizeMacroFilter(String value) {
		if (value == null) {
			return null;
		}
		String trimmed = value.trim().toLowerCase(Locale.ROOT);
		return trimmed.isEmpty() ? null : trimmed;
	}

	private boolean isNoneFilter(String macroFilter) {
		return "none".equals(macroFilter) || "no".equals(macroFilter) || "empty".equals(macroFilter);
	}

	private boolean hasMacroMembership(Unit unit) {
		if (HelloApplication.buldings == null || HelloApplication.buldings.isEmpty()) {
			return false;
		}
		for (World world : HelloApplication.buldings) {
			if (world != null && world.isUnitInside(unit)) {
				return true;
			}
		}
		return false;
	}

	private void sortFilteredUnits(ArrayList<Unit> filtered) {
		if (filtered == null || filtered.isEmpty()) {
			return;
		}
		String sortBy = sortCombo.getValue();
		if ("Health".equals(sortBy)) {
			filtered.sort((u1, u2) -> Integer.compare(
				u2 == null || u2.getHealth() == null ? 0 : u2.getHealth(),
				u1 == null || u1.getHealth() == null ? 0 : u1.getHealth()));
			return;
		}
		if ("Damage".equals(sortBy)) {
			filtered.sort((u1, u2) -> Integer.compare(
				u2 == null || u2.getDamage() == null ? 0 : u2.getDamage(),
				u1 == null || u1.getDamage() == null ? 0 : u1.getDamage()));
			return;
		}
		if ("Class".equals(sortBy)) {
			filtered.sort((u1, u2) -> Integer.compare(getHierarchyRank(u2), getHierarchyRank(u1)));
			return;
		}
		sortHelper.sortUnitsList(filtered);
	}

	private int getHierarchyRank(Unit unit) {
		if (unit == null) {
			return 0;
		}
		if (unit instanceof Pretorio) {
			return 3;
		}
		if (unit instanceof Centurio) {
			return 2;
		}
		if (unit instanceof Warrior) {
			return 1;
		}
		return 0;
	}

	private boolean isUnitInMacro(Unit unit, String macroFilter) {
		if (HelloApplication.buldings == null || HelloApplication.buldings.isEmpty()) {
			return false;
		}
		for (World world : HelloApplication.buldings) {
			if (world == null) {
				continue;
			}
			if (!world.isUnitInside(unit)) {
				continue;
			}
			String macroName = (world.name == null || world.name.isBlank())
					? world.getClass().getSimpleName()
					: world.name;
			String normalized = macroName.trim().toLowerCase(Locale.ROOT);
			if (normalized.contains(macroFilter)) {
				return true;
			}
		}
		return false;
	}

	private Integer parseIntOrNull(String value) {
		if (value == null) {
			return null;
		}
		String trimmed = value.trim();
		if (trimmed.isEmpty()) {
			return null;
		}
		try {
			return Integer.parseInt(trimmed);
		} catch (NumberFormatException ex) {
			return null;
		}
	}

	private List<String> parseInventory(String input) {
		if (input == null || input.trim().isEmpty()) {
			return new ArrayList<>();
		}
		ArrayList<String> items = new ArrayList<>();
		String[] parts = input.split(",");
		for (String part : parts) {
			if (part == null) {
				continue;
			}
			String trimmed = part.trim();
			if (!trimmed.isEmpty()) {
				items.add(trimmed.toLowerCase(Locale.ROOT));
			}
		}
		return items;
	}

	private void removeAllFiltered() {
		ArrayList<Unit> filtered = filterUnits();
		for (Unit unit : filtered) {
			if (unit != null && !Boolean.TRUE.equals(unit.getDead())) {
				unit.removeUnitFromGame();
			}
		}
		refreshList(true);
	}

	private void countMicroobjectsBySelectedParameter() {
		if (HelloApplication.units == null || HelloApplication.units.isEmpty()) {
			microobjectCountLabel.setText("Microobjects count: 0");
			return;
		}

		String parameter = countParameterCombo.getValue();
		int count = 0;

		for (Unit unit : HelloApplication.units) {
			if (unit == null || Boolean.TRUE.equals(unit.getDead())) {
				continue;
			}

			if ("moreThanHalf".equals(parameter)) {
				if (unit.moreThanHalf()) {
					count++;
				}
				continue;
			}

			if ("haveSword".equals(parameter)) {
				if (unit.haveSword()) {
					count++;
				}
				continue;
			}

			if ("team (ally)".equals(parameter)) {
				if (unit.getTeam()) {
					count++;
				}
				continue;
			}
		}

		microobjectCountLabel.setText("Microobjects count: " + count);
	}
}
