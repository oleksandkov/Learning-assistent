package org.example.laba5;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.image.Image;
import org.example.laba5.Unit.Unit;
import org.example.laba5.Unit.Warrior;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

public class GameSerializer {

    public static void save(File file, String format) {
        try {
            switch (format.toUpperCase()) {
                case "TEXT"   -> saveText(file);
                case "BINARY" -> saveBinary(file);
                case "XML"    -> saveXml(file);
                default       -> throw new IllegalArgumentException("Unknown format: " + format);
            }
            showInfo("Save successful", "Game saved to:\n" + file.getAbsolutePath());
        } catch (Exception e) {
            showError("Save failed", e.getMessage());
            e.printStackTrace();
        }
    }

    public static void load(File file, String format) {
        try {
            clearGame();
            switch (format.toUpperCase()) {
                case "TEXT"   -> loadText(file);
                case "BINARY" -> loadBinary(file);
                case "XML"    -> loadXml(file);
                default       -> throw new IllegalArgumentException("Unknown format: " + format);
            }
            bringUnitsToFront();
            showInfo("Load successful", "Game loaded from:\n" + file.getAbsolutePath());
        } catch (Exception e) {
            showError("Load failed", e.getMessage());
            e.printStackTrace();
        }
    }

    private static void saveText(File file) throws IOException {
        try (PrintWriter pw = new PrintWriter(new OutputStreamWriter(new FileOutputStream(file), StandardCharsets.UTF_8))) {
            for (Unit u : HelloApplication.units) {
                if (u == null) continue;
                pw.println("[UNIT]");
                pw.println("type="      + u.getClass().getSimpleName());
                pw.println("x="         + u.x);
                pw.println("y="         + u.y);
                pw.println("health="    + u.getHealth());
                pw.println("damage="    + u.getDamage());
                pw.println("maxHealth=" + u.getMaxHealth());
                pw.println("team="      + u.getTeam());
                pw.println("isDead="    + u.getDead());
                pw.println("isSpawned=" + u.getSpawned());
                pw.println("inventor="  + String.join(",", u.getInventor()));
                pw.println("[/UNIT]");
            }
            for (World w : HelloApplication.buldings) {
                if (w == null) continue;
                pw.println("[BUILDING]");
                pw.println("type="      + w.getClass().getSimpleName());
                pw.println("name="      + w.name);
                pw.println("x="         + w.x);
                pw.println("y="         + w.y);
                pw.println("health="    + w.getHealth());
                pw.println("maxHealth=" + w.getMaxHealth());
                pw.println("team="      + w.getTeam());
                pw.println("ore="       + w.getOre());
                pw.println("[/BUILDING]");
            }
        }
    }

    private static void loadText(File file) throws IOException {
        try (BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8))) {
            String line;
            boolean inUnit = false, inBuilding = false;
            String type = null, name = null;
            double x = 0, y = 0, maxHealth = 100, health = 100, ore = 0;
            int damage = 5;
            boolean team = true, isDead = false, isSpawned = false;
            ArrayList<String> inventor = new ArrayList<>();

            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.equals("[UNIT]")) {
                    inUnit = true;
                    type = null; x = 0; y = 0; maxHealth = 100; health = 100;
                    damage = 5; team = true; isDead = false; isSpawned = false;
                    inventor = new ArrayList<>();
                } else if (line.equals("[/UNIT]")) {
                    inUnit = false;
                    spawnLoadedUnit(type, x, y, (int) health, damage, (int) maxHealth, team, isDead, isSpawned, inventor);
                } else if (line.equals("[BUILDING]")) {
                    inBuilding = true;
                    type = null; name = ""; x = 0; y = 0; maxHealth = 200; health = 200; ore = 0; team = true;
                } else if (line.equals("[/BUILDING]")) {
                    inBuilding = false;
                    spawnLoadedBuilding(type, name, x, y, maxHealth, health, team, ore);
                } else if (inUnit || inBuilding) {
                    int eq = line.indexOf('=');
                    if (eq < 0) continue;
                    String key = line.substring(0, eq).trim();
                    String val = line.substring(eq + 1).trim();
                    switch (key) {
                        case "type"      -> type      = val;
                        case "name"      -> name      = val;
                        case "x"         -> x         = Double.parseDouble(val);
                        case "y"         -> y         = Double.parseDouble(val);
                        case "health"    -> health    = Double.parseDouble(val);
                        case "maxHealth" -> maxHealth = Double.parseDouble(val);
                        case "damage"    -> damage    = Integer.parseInt(val);
                        case "team"      -> team      = Boolean.parseBoolean(val);
                        case "isDead"    -> isDead    = Boolean.parseBoolean(val);
                        case "isSpawned" -> isSpawned = Boolean.parseBoolean(val);
                        case "ore"       -> ore       = Double.parseDouble(val);
                        case "inventor"  -> {
                            inventor = new ArrayList<>();
                            if (!val.isEmpty()) inventor.addAll(Arrays.asList(val.split(",")));
                        }
                    }
                }
            }
        }
    }

    private static final int MAGIC = 0xCAFEBABE;

    private static void saveBinary(File file) throws IOException {
        try (DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(file)))) {
            dos.writeInt(MAGIC);
            dos.writeInt(HelloApplication.units.size());
            for (Unit u : HelloApplication.units) {
                if (u == null) { dos.writeUTF("null"); continue; }
                dos.writeUTF(u.getClass().getSimpleName());
                dos.writeDouble(u.x);
                dos.writeDouble(u.y);
                dos.writeInt(u.getHealth()  != null ? u.getHealth()  : 0);
                dos.writeInt(u.getDamage()  != null ? u.getDamage()  : 0);
                dos.writeDouble(u.getMaxHealth());
                dos.writeBoolean(u.getTeam());
                dos.writeBoolean(Boolean.TRUE.equals(u.getDead()));
                dos.writeBoolean(Boolean.TRUE.equals(u.getSpawned()));
                ArrayList<String> inv = u.getInventor();
                dos.writeInt(inv == null ? 0 : inv.size());
                if (inv != null) for (String s : inv) dos.writeUTF(s);
            }
            dos.writeInt(HelloApplication.buldings.size());
            for (World w : HelloApplication.buldings) {
                if (w == null) { dos.writeUTF("null"); continue; }
                dos.writeUTF(w.getClass().getSimpleName());
                dos.writeUTF(w.name != null ? w.name : "");
                dos.writeDouble(w.x);
                dos.writeDouble(w.y);
                dos.writeDouble(w.getHealth());
                dos.writeDouble(w.getMaxHealth());
                dos.writeBoolean(w.getTeam());
                dos.writeDouble(w.getOre());
            }
        }
    }

    private static void loadBinary(File file) throws IOException {
        try (DataInputStream dis = new DataInputStream(new BufferedInputStream(new FileInputStream(file)))) {
            int magic = dis.readInt();
            if (magic != MAGIC) throw new IOException("Invalid binary save file (bad magic number).");
            int unitCount = dis.readInt();
            for (int i = 0; i < unitCount; i++) {
                String type = dis.readUTF();
                if ("null".equals(type)) continue;
                double ux       = dis.readDouble();
                double uy       = dis.readDouble();
                int health      = dis.readInt();
                int damage      = dis.readInt();
                double maxHp    = dis.readDouble();
                boolean team    = dis.readBoolean();
                boolean isDead  = dis.readBoolean();
                boolean spawned = dis.readBoolean();
                int invSize     = dis.readInt();
                ArrayList<String> inv = new ArrayList<>();
                for (int j = 0; j < invSize; j++) inv.add(dis.readUTF());
                spawnLoadedUnit(type, ux, uy, health, damage, (int) maxHp, team, isDead, spawned, inv);
            }
            int buildCount = dis.readInt();
            for (int i = 0; i < buildCount; i++) {
                String type   = dis.readUTF();
                if ("null".equals(type)) continue;
                String name   = dis.readUTF();
                double bx     = dis.readDouble();
                double by     = dis.readDouble();
                double health = dis.readDouble();
                double maxHp  = dis.readDouble();
                boolean team  = dis.readBoolean();
                double ore    = dis.readDouble();
                spawnLoadedBuilding(type, name, bx, by, maxHp, health, team, ore);
            }
        }
    }

    private static void saveXml(File file) throws Exception {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document doc = db.newDocument();

        Element root = doc.createElement("GameState");
        doc.appendChild(root);

        Element unitsEl = doc.createElement("Units");
        root.appendChild(unitsEl);
        for (Unit u : HelloApplication.units) {
            if (u == null) continue;
            Element el = doc.createElement("Unit");
            el.setAttribute("type",      u.getClass().getSimpleName());
            el.setAttribute("x",         String.valueOf(u.x));
            el.setAttribute("y",         String.valueOf(u.y));
            el.setAttribute("health",    String.valueOf(u.getHealth()));
            el.setAttribute("damage",    String.valueOf(u.getDamage()));
            el.setAttribute("maxHealth", String.valueOf(u.getMaxHealth()));
            el.setAttribute("team",      String.valueOf(u.getTeam()));
            el.setAttribute("isDead",    String.valueOf(Boolean.TRUE.equals(u.getDead())));
            el.setAttribute("isSpawned", String.valueOf(Boolean.TRUE.equals(u.getSpawned())));
            el.setAttribute("inventor",  String.join(",", u.getInventor()));
            unitsEl.appendChild(el);
        }

        Element buildingsEl = doc.createElement("Buildings");
        root.appendChild(buildingsEl);
        for (World w : HelloApplication.buldings) {
            if (w == null) continue;
            Element el = doc.createElement("Building");
            el.setAttribute("type",      w.getClass().getSimpleName());
            el.setAttribute("name",      w.name != null ? w.name : "");
            el.setAttribute("x",         String.valueOf(w.x));
            el.setAttribute("y",         String.valueOf(w.y));
            el.setAttribute("health",    String.valueOf(w.getHealth()));
            el.setAttribute("maxHealth", String.valueOf(w.getMaxHealth()));
            el.setAttribute("team",      String.valueOf(w.getTeam()));
            el.setAttribute("ore",       String.valueOf(w.getOre()));
            buildingsEl.appendChild(el);
        }

        Transformer t = TransformerFactory.newInstance().newTransformer();
        t.setOutputProperty(OutputKeys.INDENT, "yes");
        t.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");
        t.transform(new DOMSource(doc), new StreamResult(file));
    }

    private static void loadXml(File file) throws Exception {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document doc = db.parse(file);
        doc.getDocumentElement().normalize();

        NodeList unitNodes = doc.getElementsByTagName("Unit");
        for (int i = 0; i < unitNodes.getLength(); i++) {
            Element el       = (Element) unitNodes.item(i);
            String type      = el.getAttribute("type");
            double x         = Double.parseDouble(el.getAttribute("x"));
            double y         = Double.parseDouble(el.getAttribute("y"));
            int health       = Integer.parseInt(el.getAttribute("health"));
            int damage       = Integer.parseInt(el.getAttribute("damage"));
            double maxHealth = Double.parseDouble(el.getAttribute("maxHealth"));
            boolean team     = Boolean.parseBoolean(el.getAttribute("team"));
            boolean isDead   = Boolean.parseBoolean(el.getAttribute("isDead"));
            boolean spawned  = Boolean.parseBoolean(el.getAttribute("isSpawned"));
            String invStr    = el.getAttribute("inventor");
            ArrayList<String> inv = new ArrayList<>();
            if (!invStr.isEmpty()) inv.addAll(Arrays.asList(invStr.split(",")));
            spawnLoadedUnit(type, x, y, health, damage, (int) maxHealth, team, isDead, spawned, inv);
        }

        NodeList buildNodes = doc.getElementsByTagName("Building");
        for (int i = 0; i < buildNodes.getLength(); i++) {
            Element el       = (Element) buildNodes.item(i);
            String type      = el.getAttribute("type");
            String name      = el.getAttribute("name");
            double x         = Double.parseDouble(el.getAttribute("x"));
            double y         = Double.parseDouble(el.getAttribute("y"));
            double health    = Double.parseDouble(el.getAttribute("health"));
            double maxHealth = Double.parseDouble(el.getAttribute("maxHealth"));
            boolean team     = Boolean.parseBoolean(el.getAttribute("team"));
            double ore       = Double.parseDouble(el.getAttribute("ore"));
            spawnLoadedBuilding(type, name, x, y, maxHealth, health, team, ore);
        }
    }

    private static void spawnLoadedUnit(String type, double x, double y,
                                        int health, int damage, int maxHealth,
                                        boolean team, boolean isDead, boolean spawned,
                                        ArrayList<String> inventor) {
        Unit unit = switch (type) {
            case "Warrior"  -> new Warrior(health, spawned, team, damage, isDead, inventor, x, y);
            case "Centurio" -> new Centurio(health, spawned, team, damage, isDead, inventor, x, y);
            case "Pretorio" -> new Pretorio(health, spawned, team, damage, isDead, inventor, x, y);
            default -> {
                System.err.println("GameSerializer: unknown unit type: " + type);
                yield null;
            }
        };
        if (unit == null) return;
        unit.setTeam(team);
        unit.setDead(isDead);
        unit.setSpawned(spawned);
        unit.setBaseHealth(health);
        unit.setBaseDamage(damage);
        unit.setDamage(damage);
        unit.setMaxHealth(maxHealth);
        unit.setInventor(inventor);
        HelloApplication.units.add(unit);
        unit.setPosition(x, y);
        unit.resurrect();
    }

    private static void spawnLoadedBuilding(String type, String name,
                                            double x, double y,
                                            double maxHealth, double health,
                                            boolean team, double ore) {
        Image imgBase   = HelloApplication.imgBase;
        Image imgTower  = HelloApplication.imgTower;
        Image imgSource = HelloApplication.imgSource;

        World world = switch (type) {
            case "Base" -> {
                Base b = new Base();
                b.setTeam(team);
                b.initGraphics(imgBase, name, 0, x, y, maxHealth, health);
                yield b;
            }
            case "Tower" -> {
                Tower t = new Tower();
                t.setTeam(team);
                t.initGraphics(imgTower, name, 0, x, y, maxHealth, health);
                yield t;
            }
            case "Source" -> {
                Source s = new Source();
                s.setTeam(team);
                s.initGraphics(imgSource, name, 0, x, y, maxHealth, health);
                yield s;
            }
            default -> {
                System.err.println("GameSerializer: unknown building type: " + type);
                yield null;
            }
        };
        if (world == null) return;
        world.setOre(ore);
        world.resurrectWorld();
        HelloApplication.buldings.add(world);
        if (world instanceof Base b) {
            if (team) HelloApplication.basesA.add(b);
            else HelloApplication.basesB.add(b);
        } else if (world instanceof Tower t) {
            if (team) HelloApplication.towersA.add(t);
            else HelloApplication.towersB.add(t);
        } else if (world instanceof Source s) {
            if (team) HelloApplication.sourcesA.add(s);
            else HelloApplication.sourcesB.add(s);
        }
    }

    private static void bringUnitsToFront() {
        for (Unit u : HelloApplication.units) {
            if (u == null) continue;
            if (u.mainWeaponImage != null) u.mainWeaponImage.toFront();
            if (u.imageMark       != null) u.imageMark.toFront();
            if (u.life            != null) u.life.toFront();
            if (u.labelName       != null) u.labelName.toFront();
            if (u.rectActive      != null) u.rectActive.toFront();
            if (u.image           != null) u.image.toFront();
            if (u.getOreCountLabel()  != null) u.getOreCountLabel().toFront();
            if (u.getKillCountLabel() != null) u.getKillCountLabel().toFront();
        }
    }

    private static void clearGame() {
        for (Unit u : new ArrayList<>(HelloApplication.units)) {
            if (u != null) u.removeUnitFromGame();
        }
        HelloApplication.units.clear();
        for (World w : new ArrayList<>(HelloApplication.buldings)) {
            if (w != null) w.removeBuildingFromGame();
        }
        HelloApplication.buldings.clear();
        HelloApplication.basesA.clear();
        HelloApplication.basesB.clear();
        HelloApplication.towersA.clear();
        HelloApplication.towersB.clear();
        HelloApplication.sourcesA.clear();
        HelloApplication.sourcesB.clear();
    }

    private static void showInfo(String title, String msg) {
        Platform.runLater(() -> {
            Alert a = new Alert(Alert.AlertType.INFORMATION);
            a.setTitle(title);
            a.setHeaderText(null);
            a.setContentText(msg);
            a.showAndWait();
        });
    }

    private static void showError(String title, String msg) {
        Platform.runLater(() -> {
            Alert a = new Alert(Alert.AlertType.ERROR);
            a.setTitle(title);
            a.setHeaderText(null);
            a.setContentText(msg != null ? msg : "Unknown error");
            a.showAndWait();
        });
    }
}
