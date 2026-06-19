package org.example.laba5.Unit;

import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Objects;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Line;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import org.example.laba5.HelloApplication;
import org.example.laba5.World;

public class Unit implements Cloneable {
    private Integer health;
    private Boolean isSpawned;
    protected boolean team;
    private Integer damage;
    private Boolean isDead;
    private ArrayList<String> inventor;

    private Integer baseHealth;
    private Integer baseDamage;

    private static int numObjects = 0;
    private static int objectedKilled = 0;

    public Label labelName;
    public Line life;
    public ImageView image;
    public double x, y;
    public boolean isActive;
    public Rectangle rectActive;
    public double maxHealth;
    public ImageView imageMarkRed;
    public ImageView imageMarkGreen;
    public ImageView imageMark;
    public ImageView swordImage;
    public ImageView knifeImage;
    public ImageView spearImage;
    public ImageView bowImage;
    public ImageView mainWeaponImage;

    protected double moveSpeed = 2;
    private static boolean isPushing = false;

    protected long lastAttackTime = 0;
    protected final long ATTACK_COOLDOWN = 1000;
    public static double MAX_HEALTH;

    URL swordUrl = getClass().getResource("/sword.png");
    URL knifeUrl = getClass().getResource("/knife.png");
    URL spearUrl = getClass().getResource("/spear.png");
    URL bowUrl = getClass().getResource("/bow.png");

    public static void plusObjectedKilled() {
        objectedKilled++;
    }

    public void setMaxHealth(double maxHealth) {
        this.maxHealth = maxHealth;
    }
    
    public double getMaxHealth() {
        return this.maxHealth;
    }

    protected double labelDeltaX() {
        return 10.0;
    }

    protected double labelDeltaY() {
        return -10.0;
    }

    protected double lifeDeltaX() {
        return 0.0;
    }

    protected double lifeDeltaY() {
        return 10.0;
    }

    protected double imageDeltaX() {
        return 0.0;
    }

    protected double imageDeltaY() {
        return 0.0;
    }

    protected double rectDeltaX() {
        return -9.0;
    }

    protected double rectDeltaY() {
        return -9.0;
    }

    public boolean isActive() {
        return isActive;
    }

    public Unit(Integer health, Boolean isSpawned, boolean team, Integer damage, Boolean isDead, ArrayList<String> inventor) {
        this.health = health;
        this.baseHealth = health;
        this.isSpawned = isSpawned;
        this.team = team;
        this.damage = damage;
        this.baseDamage = damage;
        this.isDead = isDead;
        this.inventor = inventor;
        numObjects++;
    }

    public Unit() {
        this(100, false, true, 5, false, new ArrayList<String>(Arrays.asList("sword")));
    }

    public boolean getActive() {
        return isActive;
    }

    public static int getNumObjects() {
        return numObjects;
    }

    public static void setNumObjects(int num) {
        numObjects = num;
    }

    public Integer getHealth() {
        return health;
    }

    public Boolean getSpawned() {
        return isSpawned;
    }

    public boolean getTeam() {
        return team;
    }

    public Integer getDamage() {
        return damage;
    }

    public Boolean getDead() {
        return isDead;
    }

    public ArrayList<String> getInventor() {
        return inventor;
    }

    public static int getnumObjects() {
        return numObjects;
    }

    public javafx.scene.image.ImageView getImage() {
        return image;
    }

    public double getX() {
        return x;
    }

    public double getY() {
        return y;
    }

    public static class HealthComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return Integer.compare(a.getHealth(), b.getHealth());
        }
    }

    public static class TeamComparator implements Comparator<Unit> {
        @Override
        public int compare(Unit a, Unit b) {
            return Boolean.compare(a.getTeam(), b.getTeam());
        }
    }

    public int compareTo47(Unit x) {
        int cmp = Integer.compare(this.health, x.health);
        if (cmp != 0) return cmp;
        cmp = Boolean.compare(this.team, x.team);
        if (cmp != 0) return cmp;
        cmp = this.damage.compareTo(x.damage);
        if (cmp != 0) return cmp;
        cmp = this.isSpawned.compareTo(x.isSpawned);
        if (cmp != 0) return cmp;
        cmp = this.isDead.compareTo(x.isDead);
        if (cmp != 0) return cmp;
        ArrayList<String> sortedInventor1 = new ArrayList<>(this.inventor);
        ArrayList<String> sortedInventor2 = new ArrayList<>(x.inventor);
        sortedInventor1.sort(String::compareTo);
        sortedInventor2.sort(String::compareTo);
        cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
        if (cmp != 0) return cmp;
        return 0;
    }

    public static Comparator<Unit> comparatorFromTemplate(Unit template) {
        return (u1, u2) -> {
            if (template == null) {
                throw new IllegalArgumentException("Template cannot be null");
            }
            int cmp;
            if (template.getHealth() != null) {
                cmp = Integer.compare(u1.getHealth(), u2.getHealth());
                if (cmp != 0) return cmp;
            }
            cmp = Boolean.compare(u1.getTeam(), u2.getTeam());
            if (cmp != 0) return cmp;
            if (template.getDamage() != null) {
                cmp = u1.getDamage().compareTo(u2.getDamage());
                if (cmp != 0) return cmp;
            }
            if (template.getSpawned() != null) {
                cmp = u1.getSpawned().compareTo(u2.getSpawned());
                if (cmp != 0) return cmp;
            }
            if (template.getDead() != null) {
                cmp = u1.getDead().compareTo(u2.getDead());
                if (cmp != 0) return cmp;
            }
            if (template.getInventor() != null) {
                ArrayList<String> sortedInventor1 = new ArrayList<>(u1.getInventor());
                ArrayList<String> sortedInventor2 = new ArrayList<>(u2.getInventor());
                sortedInventor1.sort(String::compareTo);
                sortedInventor2.sort(String::compareTo);
                cmp = sortedInventor1.toString().compareTo(sortedInventor2.toString());
                if (cmp != 0) return cmp;
            }
            return 0;
        };
    }

    public void setHealth(Integer health) {
        this.health = health;
        if (this.health != null && this.health < 0) {
            removeUnitFromGame();
            return;
        }
        if (life != null) {
            setCoordinates();
        }
    }
    
    public void removeUnitFromGame() {
        this.isDead = true;
        if (HelloApplication.group != null) {
            if (this.labelName != null) {
                HelloApplication.group.getChildren().remove(this.labelName);
            }
            if (this.life != null) {
                HelloApplication.group.getChildren().remove(this.life);
            }
            if (this.image != null) {
                HelloApplication.group.getChildren().remove(this.image);
            }
            if (this.rectActive != null) {
                HelloApplication.group.getChildren().remove(this.rectActive);
            }
            if (this.imageMark != null) {
                HelloApplication.group.getChildren().remove(this.imageMark);
            }
            if (this.mainWeaponImage != null) {
                HelloApplication.group.getChildren().remove(this.mainWeaponImage);
            }
            if (this.getOreCountLabel() != null) {
                HelloApplication.group.getChildren().remove(this.getOreCountLabel());
            }
            if (this.getKillCountLabel() != null) {
                HelloApplication.group.getChildren().remove(this.getKillCountLabel());
            }
        }
        if (HelloApplication.units != null) {
            HelloApplication.units.remove(this);
        }
        removeUnit();
    }

    public void setSpawned(Boolean spawned) {
        isSpawned = spawned;
    }

    public void setTeam(boolean team) {
        this.team = team;
    }

    public void setDamage(Integer damage) {
        this.damage = damage;
    }

    public void setBaseHealth(Integer baseHealth) {
        this.baseHealth = baseHealth;
    }

    public void setBaseDamage(Integer baseDamage) {
        this.baseDamage = baseDamage;
    }

    public Integer getBaseHealth() {
        return baseHealth;
    }

    public Integer getBaseDamage() {
        return baseDamage;
    }

    public void setDead(Boolean dead) {
        isDead = dead;
    }

    public void setInventor(ArrayList<String> inventor) {
        this.inventor = inventor;
        inventoryLogic();
    }

    @Override
    public boolean equals(Object o) {
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return Objects.equals(health, unit.health) && Objects.equals(isSpawned, unit.isSpawned) && team == unit.team && Objects.equals(damage, unit.damage) && Objects.equals(isDead, unit.isDead) && Objects.equals(inventor, unit.inventor);
    }

    @Override
    public int hashCode() {
        return Objects.hash(health, isSpawned, team, damage, isDead, inventor);
    }

    @Override
    public String toString() {
        return "Unit{" +
                "health=" + health +
                ", isSpawned=" + isSpawned +
                ", Team='" + team + '\'' +
                ", damage=" + damage +
                ", isDead=" + isDead +
                ", inventor=" + inventor +
                "}\n";
    }

    @Override
    public Unit clone() throws CloneNotSupportedException {
        Unit clonedUnit = (Unit) super.clone();
        ArrayList<String> clonedInventor = this.inventor;

        if (this.labelName != null) {
            clonedUnit.labelName = new Label(this.labelName.getText());
        }
        if (this.life != null) {
            clonedUnit.life = new Line();
            clonedUnit.life.setStrokeWidth(this.life.getStrokeWidth());
            clonedUnit.life.setStroke(this.life.getStroke());
        }
        if (this.image != null) {
            clonedUnit.image = new ImageView(this.image.getImage());
            clonedUnit.image.setFitWidth(this.image.getFitWidth());
            clonedUnit.image.setFitHeight(this.image.getFitHeight());
        }
        if (this.rectActive != null) {
            double rectWidth = this.rectActive.getWidth();
            double rectHeight = this.rectActive.getHeight();
            clonedUnit.rectActive = new Rectangle(rectWidth, rectHeight);
            clonedUnit.rectActive.setFill(this.rectActive.getFill());
            clonedUnit.rectActive.setStroke(this.rectActive.getStroke());
            clonedUnit.rectActive.setStrokeWidth(this.rectActive.getStrokeWidth());
        }
        if (this.imageMarkRed != null) {
            clonedUnit.imageMarkRed = new ImageView(this.imageMarkRed.getImage());
            clonedUnit.imageMarkRed.setFitWidth(this.imageMarkRed.getFitWidth());
            clonedUnit.imageMarkRed.setFitHeight(this.imageMarkRed.getFitHeight());
        }
        if (this.imageMarkGreen != null) {
            clonedUnit.imageMarkGreen = new ImageView(this.imageMarkGreen.getImage());
            clonedUnit.imageMarkGreen.setFitWidth(this.imageMarkGreen.getFitWidth());
            clonedUnit.imageMarkGreen.setFitHeight(this.imageMarkGreen.getFitHeight());
        }
        if (this.mainWeaponImage != null) {
            clonedUnit.mainWeaponImage = new ImageView(this.mainWeaponImage.getImage());
            clonedUnit.mainWeaponImage.setFitWidth(this.mainWeaponImage.getFitWidth());
            clonedUnit.mainWeaponImage.setFitHeight(this.mainWeaponImage.getFitHeight());
        }

        clonedUnit.baseHealth = this.baseHealth;
        clonedUnit.baseDamage = this.baseDamage;
        clonedUnit.setInventor(clonedInventor);
        clonedUnit.isActive = false;
        clonedUnit.maxHealth = this.maxHealth;
        numObjects++;
        return clonedUnit;
    }

    public void attack() {
        boolean intersects = false;
        if (this.damage == null || this.damage <= 0) {
            return;
        }
        if (this.image == null) {
            return;
        }
        if (HelloApplication.units != null) {
            for (Unit unit : HelloApplication.units) {
                if (unit != this && unit.getTeam() != this.team && !unit.getDead() && unit.image != null) {
                    intersects = this.image.getBoundsInParent().intersects(unit.image.getBoundsInParent());
                    if (intersects) {
                        int targetHealth = unit.getHealth() == null ? 0 : unit.getHealth();
                        int newHealth = targetHealth - this.damage;
                        unit.setHealth(newHealth);
                        if (newHealth <= 0) {
                            unit.setHealth(0);
                            objectedKilled++;
                            unit.removeUnitFromGame();
                        }
                        break;
                    }
                }
            }
        }
        if (HelloApplication.buildings != null) {
            for (World world : HelloApplication.buildings) {
                if (world != null && world.getTeam() != this.team && world.getImageView() != null) {
                    intersects = this.image.getBoundsInParent().intersects(world.getImageView().getBoundsInParent());
                    if (intersects) {
                        int targetHealth = world.getHealth() == 0 ? 0 : (int) world.getHealth();
                        int newHealth = targetHealth - this.damage;
                        world.setHealth(newHealth);
                    }
                }
            }
        }
    }

    public static void removeUnit() {
        numObjects--;
    }

    public void addHealth() {
        this.health += 10;
    }

    public void addDamage() {
        this.damage += 5;
    }

    public void changeTeam() {
        this.team = !this.team;
    }

    public boolean isAlly() {
        return this.team;
    }

    public boolean isAlly(Unit x) {
        return x != null && this.team == x.team;
    }

    public void takeDamage() {
        this.health -= 10;
    }

    public void showTheStrongest(Unit unit) {
        Unit maxhealth = (this.health > unit.health) ? this : unit;
    }

    public void spawnAtTeamBase() {
        if (this.getTeam()) {
            if (HelloApplication.basesA != null && !HelloApplication.basesA.isEmpty()) {
                setPosition(HelloApplication.basesA.get(0).getX(), HelloApplication.basesA.get(0).getY());
            }
        } else {
            if (HelloApplication.basesB != null && !HelloApplication.basesB.isEmpty()) {
                setPosition(HelloApplication.basesB.get(0).getX(), HelloApplication.basesB.get(0).getY());
            }
        }
    }

    public void resurrect() {
        if (HelloApplication.group == null || labelName == null || life == null || image == null) {
            return;
        }
        loadMarkImages();
        if (this.getTeam()) {
            imageMark = imageMarkGreen;
        } else {
            imageMark = imageMarkRed;
        }
        HelloApplication.group.getChildren().addAll(labelName, life, image);
        if (imageMark != null) {
            HelloApplication.group.getChildren().add(imageMark);
        }
        if (mainWeaponImage != null) {
            HelloApplication.group.getChildren().add(mainWeaponImage);
        }
        if (isActive) {
            HelloApplication.group.getChildren().add(rectActive);
        }
        setCoordinates();
        inventoryLogic();
    }

    public void loadInventoryImages() {
        try {
            if (swordImage == null && swordUrl != null) {
                swordImage = new ImageView(new Image(swordUrl.toExternalForm()));
                swordImage.setFitWidth(40);
                swordImage.setFitHeight(40);
                swordImage.setPreserveRatio(true);
            }
            if (knifeImage == null && knifeUrl != null) {
                knifeImage = new ImageView(new Image(knifeUrl.toExternalForm()));
                knifeImage.setFitWidth(40);
                knifeImage.setFitHeight(40);
                knifeImage.setPreserveRatio(true);
            }
            if (spearImage == null && spearUrl != null) {
                spearImage = new ImageView(new Image(spearUrl.toExternalForm()));
                spearImage.setFitWidth(40);
                spearImage.setFitHeight(40);
                spearImage.setPreserveRatio(true);
            }
            if (bowImage == null && bowUrl != null) {
                bowImage = new ImageView(new Image(bowUrl.toExternalForm()));
                bowImage.setFitWidth(40);
                bowImage.setFitHeight(40);
                bowImage.setPreserveRatio(true);
            }
        } catch (Exception e) {
        }
    }

    public void loadMarkImages() {
        if (imageMarkRed != null && imageMarkGreen != null) {
            return;
        }
        try {
            URL redUrl = getClass().getResource("/red.png");
            URL greenUrl = getClass().getResource("/green.png");
            if (redUrl != null) {
                imageMarkRed = new ImageView(new Image(redUrl.toExternalForm()));
                imageMarkRed.setFitWidth(20);
                imageMarkRed.setFitHeight(20);
            }
            if (greenUrl != null) {
                imageMarkGreen = new ImageView(new Image(greenUrl.toExternalForm()));
                imageMarkGreen.setFitWidth(20);
                imageMarkGreen.setFitHeight(20);
            }
        } catch (Exception e) {
        }
    }

    public void clampBounds() {
        double maxW = HelloApplication.WORLD_WIDTH;
        double maxH = HelloApplication.WORLD_HEIGHT;
        double fitW = image != null ? image.getFitWidth() : 100.0;
        double fitH = image != null ? image.getFitHeight() : 100.0;

        if (x < 0) x = 0;
        if (x > maxW - fitW) x = maxW - fitW;
        if (y < 0) y = 0;
        if (y > maxH - fitH) y = maxH - fitH;
    }

    public void setCoordinates() {
        clampBounds();
        if (labelName == null || life == null || image == null) {
            return;
        }
        labelName.setLayoutX(x + labelDeltaX());
        labelName.setLayoutY(y + labelDeltaY());

        double hp = getHealth() == null ? 0.0 : Math.max(0.0, getHealth());
        double effectiveMaxHealth = maxHealth > 0.0 ? maxHealth : 100.0;
        double lifeBaseX = x + lifeDeltaX();
        double lifeBaseY = y + lifeDeltaY();
        life.setStartX(lifeBaseX);
        life.setStartY(lifeBaseY);
        life.setEndX(lifeBaseX + Math.min((hp / effectiveMaxHealth) * 100, 100));
        life.setEndY(lifeBaseY);

        image.setX(x + imageDeltaX());
        image.setY(y + imageDeltaY());

        if (rectActive != null) {
            rectActive.setX(x + rectDeltaX());
            rectActive.setY(y + rectDeltaY());
        }

        if (imageMark != null) {
            imageMark.setX(x + image.getFitWidth() - 20);
            imageMark.setY(y + image.getFitHeight() - 20);
        }
        if (mainWeaponImage != null) {
            mainWeaponImage.setX(x);
            mainWeaponImage.setY(y + 10);
        }
    }

    public void move(double dx, double dy) {
        x += dx;
        y += dy;
        clampBounds();
        setCoordinates();
        locateAndRotateF();
        locateAndRotateE();
    }

    public void locateAndRotateF() {
        if (isPushing) {
            return;
        }
        if (HelloApplication.units == null || image == null) {
            return;
        }
        try {
            isPushing = true;
            double selfCenterX = x + image.getFitWidth() / 2.0;
            double selfCenterY = y + image.getFitHeight() / 2.0;
            double minDistance = 30.0;        
            double pushDistance = 50.0;       
            for (Unit unit : HelloApplication.units) {
                if (unit == this || unit.image == null) {
                    continue;
                }
                if (unit.getTeam() != this.team) {
                    continue;
                }
                double otherCenterX = unit.x + unit.image.getFitWidth() / 2.0;
                double otherCenterY = unit.y + unit.image.getFitHeight() / 2.0;
                double dx = otherCenterX - selfCenterX;
                double dy = otherCenterY - selfCenterY;
                double dist = Math.hypot(dx, dy);

                if (dist < minDistance) {
                    if (dist < 0.001) {
                        dx = 1.0;
                        dy = 0.0;
                        dist = 1.0;
                    }
                    double newX = selfCenterX + (dx / dist) * pushDistance;
                    double newY = selfCenterY + (dy / dist) * pushDistance;
                    unit.moveTo(newX - unit.image.getFitWidth() / 2.0, newY - unit.image.getFitHeight() / 2.0);
                }
            }
        } finally {
            isPushing = false;
        }
    }

    public void locateAndRotateE() {
    }

    public void moveTo(double newX, double newY) {
        double dx = newX - x;
        double dy = newY - y;
        double dist = Math.hypot(dx, dy);

        if (dist <= moveSpeed) {
            x = newX;
            y = newY;
        } else {
            move(dx / dist * moveSpeed, dy / dist * moveSpeed);
            return;
        }
        clampBounds();
        locateAndRotateE();
        locateAndRotateF();
        setCoordinates();
    }

    public void setPosition(double newX, double newY) {
        x = newX;
        y = newY;
        clampBounds();
        locateAndRotateE();
        locateAndRotateF();
        setCoordinates();
    }

    public boolean flipActivation() {
        if (HelloApplication.group != null) {
            if (isActive) {
                HelloApplication.group.getChildren().remove(rectActive);
            } else {
                HelloApplication.group.getChildren().add(rectActive);
            }
        }
        isActive = !isActive;
        return isActive;
    }

    public boolean tryActivate(double mx, double my) {
        if (image.getBoundsInParent().contains(mx, my)) {
            flipActivation();
            return true;
        }
        return false;
    }

    public void updateTeamMark() {
        loadMarkImages();
        if (imageMark != null && HelloApplication.group.getChildren().contains(imageMark)) {
            HelloApplication.group.getChildren().remove(imageMark);
        }
        if (this.team) {
            imageMark = imageMarkGreen;
        } else {
            imageMark = imageMarkRed;
        }
        if (imageMark != null) {
            HelloApplication.group.getChildren().add(imageMark);
        }
        setCoordinates();
    }

    public void logic() {
    }

    protected void inventoryLogic() {
        loadInventoryImages();
        if (HelloApplication.group != null && mainWeaponImage != null && HelloApplication.group.getChildren().contains(mainWeaponImage)) {
            HelloApplication.group.getChildren().remove(mainWeaponImage);
        }
        mainWeaponImage = null;

        if (baseHealth != null) {
            this.health = baseHealth;
        }
        if (baseDamage != null) {
            this.damage = baseDamage;
        }
        this.maxHealth = baseHealth != null ? baseHealth : 100.0;

        if (this.inventor == null || this.inventor.isEmpty()) {
            this.setInventor(new ArrayList<>(Arrays.asList("Spear")));
            setCoordinates();
            return;
        }

        String mainWeapon = this.inventor.get(0);
        int currentDamage = this.getDamage() != null ? this.getDamage() : 0;

        if (mainWeapon.equalsIgnoreCase("sword")) {
            this.damage = currentDamage + 5;
            if (swordImage != null && swordImage.getImage() != null) mainWeaponImage = new ImageView(swordImage.getImage());
        } else if (mainWeapon.equalsIgnoreCase("knife")) {
            this.damage = currentDamage + 3;
            if (knifeImage != null && knifeImage.getImage() != null) mainWeaponImage = new ImageView(knifeImage.getImage());
        } else if (mainWeapon.equalsIgnoreCase("spear")) {
            this.damage = currentDamage + 4;
            if (spearImage != null && spearImage.getImage() != null) mainWeaponImage = new ImageView(spearImage.getImage());
        } else if (mainWeapon.equalsIgnoreCase("bow")) {
            this.damage = currentDamage + 2;
            if (bowImage != null && bowImage.getImage() != null) mainWeaponImage = new ImageView(bowImage.getImage());
        }

        if (mainWeaponImage != null) {
            mainWeaponImage.setFitWidth(40);
            mainWeaponImage.setFitHeight(40);
            mainWeaponImage.setPreserveRatio(true);
            if (HelloApplication.group != null) HelloApplication.group.getChildren().add(mainWeaponImage);
        }

        for (int i = 1; i < this.inventor.size(); i++) {
            String item = this.inventor.get(i);
            if (item == null) continue;
            String it = item.trim().toLowerCase();
            if (it.equals("health potion") || it.equals("health_potion") || it.equals("health")) {
                this.maxHealth += 20;
                Integer curHp = this.getHealth();
                this.health = (curHp == null ? 20 : curHp + 20);
            } else if (it.equals("damage potion") || it.equals("damage_potion") || it.equals("damage")) {
                Integer curDmg = this.getDamage();
                this.damage = (curDmg == null ? 0 : curDmg) + 5;
            }
        }
        setCoordinates();
    }

    protected void promotion() {
    }

    public javafx.scene.control.Label getOreCountLabel() {
        return null;
    }

    public void setOreCount(int oreCount) {
    }

    public javafx.scene.control.Label getKillCountLabel() {
        return null;
    }

    protected void logicInverse() {
    }

    public boolean moreThanHalf() {
        return this.health != null && this.maxHealth > 0 && this.health > this.maxHealth / 2;
    }

    public boolean haveSword() {
        if (this.inventor != null) {
            for (String item : this.inventor) {
                if (item != null && item.trim().equalsIgnoreCase("sword")) {
                    return true;
                }
            }
        }
        return false;
    }
}