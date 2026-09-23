import { test, expect } from "@playwright/test";

test("flybrain streams real frames, pauses, replays and keeps the API web-only", async ({ page, request }) => {
  const errors: string[] = [];
  page.on("pageerror", error => errors.push(error.message));
  await page.goto("/play/02-microban");
  await page.getByLabel("Алгоритм", { exact: true }).selectOption("flybrain");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  const live = page.locator(".flybrain-live .flybrain-viewer");
  await expect.poll(async () => Number(await live.getAttribute("data-frames")), { timeout: 15000 }).toBeGreaterThanOrEqual(10);
  await live.getByRole("button", { name: "Пауза", exact: true }).click();
  await live.getByLabel("Крок FlyBrain", { exact: true }).fill("2");
  await live.getByRole("button", { name: "FlyBrain: наступний крок" }).click();
  await expect(live).toHaveAttribute("data-step", "3");
  const card = page.locator(".decision-card").filter({ hasText: "11 - FlyBrain" });
  await expect(card).toBeVisible({ timeout: 30000 });
  const viewer = card.locator(".flybrain-viewer");
  await viewer.getByLabel("Крок FlyBrain", { exact: true }).fill("10");
  await expect(viewer.locator("canvas")).toBeVisible();
  await expect(viewer.locator(".game-board")).toBeVisible();
  await expect(card.getByLabel("Порівнювати 11 - FlyBrain")).toBeDisabled();
  expect(await viewer.evaluate(el => el.clientWidth)).toBeGreaterThan(900);
  await viewer.screenshot({ path: "test-results/flybrain-desktop.png" });
  await page.setViewportSize({ width: 390, height: 844 });
  expect(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth)).toBe(true);
  await viewer.screenshot({ path: "test-results/flybrain-mobile.png" });
  const rejected = await request.post("/api/game", { data: { levelId: "01-simple", history: "", mode: "solve", algorithm: "flybrain" } });
  expect(rejected.status()).toBe(400);
  expect(errors).toEqual([]);
});

test("flybrain cancellation terminates the worker and selection keeps manual moves working", async ({ page }) => {
  await page.emulateMedia({ reducedMotion: "reduce" });
  await page.goto("/play/02-microban");
  await page.getByLabel("Алгоритм", { exact: true }).selectOption("flybrain");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  await expect(page.locator(".flybrain-live")).toBeVisible();
  await expect(page.locator(".flybrain-live .flybrain-viewer")).toHaveAttribute("data-step", "0");
  await page.locator(".flybrain-viewer").getByRole("button", { name: "Скасувати пошук" }).click();
  await expect(page.locator(".flybrain-live")).toHaveCount(0);
  const stateResponse = page.waitForResponse(r => r.url().endsWith("/api/game") && r.request().postDataJSON().mode === "state");
  await page.locator("h1").click();
  await page.keyboard.press("ArrowRight");
  expect((await stateResponse).status()).toBe(200);
  await expect(page.locator(".decision-card")).toHaveCount(0);
});

test("flybrain one-step solution restores and is validated again before applying", async ({ page }) => {
  await page.goto("/play/01-simple");
  await page.getByLabel("Алгоритм", { exact: true }).selectOption("flybrain");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  let card = page.locator(".decision-card").filter({ hasText: "11 - FlyBrain" });
  await expect(card.locator(".decision-status")).toHaveText("Розв’язано");
  await page.reload();
  card = page.locator(".decision-card").filter({ hasText: "11 - FlyBrain" });
  await expect(card.locator(".flybrain-viewer")).toHaveAttribute("data-frames", "1");
  await card.getByRole("button", { name: "Обчислити A* з тієї ж позиції" }).click();
  await expect(card.locator(".flybrain-footnote strong")).toContainText("A* 1 vs FlyBrain 1");
  const checked = page.waitForResponse(r => r.url().endsWith("/api/game") &&
    r.request().postDataJSON().history === "U" && r.request().postDataJSON().direction === "");
  await card.getByRole("button", { name: "Застосувати", exact: true }).click();
  expect((await checked).status()).toBe(200);
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено");
  await card.locator(".flybrain-viewer").screenshot({ path: "test-results/flybrain-simple.png" });
});

test("flybrain lesson is discoverable, explains all six phases and respects reduced motion", async ({ page }) => {
  await page.goto("/about");
  await page.getByRole("link", { name: "Прототип 8 · FlyBrain", exact: true }).click();
  await expect(page).toHaveURL(/\/about\/flybrain$/);
  await expect(page.locator(".lesson h2")).toHaveText("FlyBrain: як імпульс стає рухом");
  for (let i = 0; i < 6; i++) {
    await page.getByLabel(`Крок ${i + 1}`, { exact: true }).click();
    await expect(page.locator(".flybrain-lesson")).toHaveAttribute("data-phase", String(i));
    await expect(page.locator(".lesson-source summary")).toContainText("TypeScript");
  }
  await expect(page.locator(".fb-teaching-box")).toHaveClass(/on-goal/);
  await page.locator(".demo").screenshot({ path: "test-results/flybrain-lesson.png" });
  await page.emulateMedia({ reducedMotion: "reduce", colorScheme: "dark" });
  await page.setViewportSize({ width: 390, height: 844 });
  await page.getByLabel("Крок 3", { exact: true }).click();
  expect(await page.locator(".fb-signal").evaluate(el => getComputedStyle(el).transitionDuration)).toBe("0s");
  expect(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth)).toBe(true);
  await page.locator(".demo").screenshot({ path: "test-results/flybrain-lesson-mobile.png" });
});

test("flybrain 3D stays open during in-place recalculation and configurable automatic retries", async ({ page }) => {
  const errors: string[] = [];
  page.on("pageerror", e => errors.push(e.message));
  await page.goto("/play/03-microban-3");
  await page.getByLabel("Алгоритм", { exact: true }).selectOption("flybrain");
  await page.getByLabel("Ліміт кроків FlyBrain").fill("20");
  await page.getByLabel("Вікно нейронів, мс").fill("24");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  const viewer = page.locator(".flybrain-viewer");
  await expect(viewer).toHaveAttribute("data-frames", "20");
  await expect(viewer).toContainText("вікно 24 мс");
  await expect(page.locator(".flybrain-live")).toHaveCount(0);
  await viewer.evaluate(el => el.setAttribute("data-persistent", "yes"));
  await viewer.getByRole("button", { name: "Відкрити 3D-мозок" }).click();
  const dialog = page.getByRole("dialog", { name: "3D-мозок FlyBrain" });
  await expect(dialog).toBeVisible();
  const model = dialog.locator("canvas");
  await model.focus(); await page.keyboard.press("ArrowRight");
  await dialog.getByLabel("Масштаб 3D").fill("1.25");
  await dialog.screenshot({ path: "test-results/flybrain-3d-desktop.png" });
  await dialog.getByRole("button", { name: "Перерахувати тут" }).click();
  await expect(viewer).toHaveAttribute("data-seed", "9");
  await expect(viewer).toHaveAttribute("data-persistent", "yes");
  await expect(page.locator(".flybrain-viewer")).toHaveCount(1);
  await expect(dialog).toBeVisible();
  await expect(dialog.getByLabel("Масштаб 3D")).toHaveValue("1.25");
  await expect(page.locator(".flybrain-live")).toHaveCount(0);
  await page.keyboard.press("Escape");
  await expect(dialog).not.toBeVisible();
  await page.getByLabel("Ліміт кроків FlyBrain").fill("1");
  await viewer.locator(".flybrain-run-controls").getByLabel("Повторювати до розв’язання").check();
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  await expect.poll(async () => Number(await viewer.getAttribute("data-attempt"))).toBeGreaterThanOrEqual(2);
  await expect(viewer).toHaveAttribute("data-persistent", "yes");
  await viewer.getByRole("button", { name: "Відкрити 3D-мозок" }).click();
  await dialog.getByRole("button", { name: "Скасувати пошук" }).click();
  await expect(page.locator(".flybrain-live")).toHaveCount(0);
  await expect(dialog).toBeVisible();
  const seed = await viewer.getAttribute("data-seed");
  await page.setViewportSize({ width: 390, height: 844 });
  await dialog.screenshot({ path: "test-results/flybrain-3d-mobile.png" });
  await expect(viewer).toHaveAttribute("data-seed", seed!);
  expect(await dialog.evaluate(el => el.scrollWidth <= el.clientWidth + 1)).toBe(true);
  expect(errors).toEqual([]);
});

test("flybrain retries from the card stop at a C++-confirmed win and show the winning board", async ({ page }) => {
  await page.goto("/play/01-simple");
  await page.getByLabel("Алгоритм", { exact: true }).selectOption("flybrain");
  await page.getByLabel("Ліміт кроків FlyBrain").fill("1");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  const viewer = page.locator(".flybrain-viewer"), controls = viewer.locator(".flybrain-run-controls");
  await expect(viewer.locator(".flybrain-solved")).toBeVisible();
  await controls.getByLabel("Повторювати до розв’язання").check();
  await controls.getByRole("button", { name: "Перерахувати тут" }).click();
  await expect(viewer).not.toHaveAttribute("data-seed", "8");
  await expect(viewer.locator(".flybrain-solved")).toContainText("усі ящики на цілях", { timeout: 15000 });
  await expect(page.locator(".flybrain-live")).toHaveCount(0);
  await expect(viewer).toHaveAttribute("data-step", "1");
  await expect(viewer.locator(".actor.box.on-goal")).toHaveCount(1);
  await expect(controls.getByLabel("Повторювати до розв’язання")).toBeChecked();
  const seed = await viewer.getAttribute("data-seed");
  await page.clock.install(); await page.clock.runFor(5000);
  await expect(viewer).toHaveAttribute("data-seed", seed!);
  await viewer.screenshot({ path: "test-results/flybrain-card-solved.png" });
});
