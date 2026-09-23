import { test, expect } from "@playwright/test";

test("selected routes advance together without moving the human board", async ({ page }) => {
  await page.setViewportSize({ width: 856, height: 935 });
  await page.emulateMedia({ colorScheme: "dark" });
  await page.goto("/play/04-small");
  await expect(page.locator(".play-area .game-board")).toBeVisible();
  await page.getByRole("button", { name: "Порівняти всі алгоритми" }).click();
  await expect(page.locator(".decision-card")).toHaveCount(10);
  await page.getByLabel("Порівнювати 1 - BFS", { exact: true }).check();
  await page.getByLabel("Порівнювати 2 - A* ходи", { exact: true }).check();
  await page.getByRole("button", { name: "Порівняти вибрані поруч" }).click();
  const comparison = page.getByRole("region", { name: "Синхронне порівняння" });
  await expect(comparison.locator(".game-board")).toHaveCount(2);
  await comparison.getByLabel("Порівняння: наступний крок").click();
  await expect(comparison.locator("article header span").first()).toHaveText("1 / 7");
  await expect(comparison.locator("article header span").last()).toHaveText("1 / 7");
  await expect(page.getByTestId("moves")).toHaveText("0");
  await comparison.getByLabel("Порівняння: попередній крок").click();
  await expect(comparison.locator("article header span").first()).toHaveText("0 / 7");
  await comparison.getByRole("button", { name: "Запустити разом" }).click();
  await expect(comparison.locator("article header span")).toHaveText(["Завершено", "Завершено"]);
  // Verify "На початок" resets the replay from the completed state:
  const restartBtn = comparison.getByRole("button", { name: "На початок" });
  await expect(restartBtn).toBeEnabled();
  await restartBtn.click();
  await expect(comparison.locator("article header span").first()).toHaveText("0 / 7");
  await expect(comparison.locator("article header span").last()).toHaveText("0 / 7");
  await expect(restartBtn).toBeDisabled();
  await expect(page.locator(".success, details.help")).toHaveCount(0);
  await page.locator(".algorithm-workbench").screenshot({ path: "test-results/comparison-minimal-dark.png" });
  await page.setViewportSize({ width: 390, height: 844 });
  expect(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth)).toBe(true);
});

test("comparing all algorithms on a completed level computes solutions from the initial position", async ({ page }) => {
  await page.setViewportSize({ width: 930, height: 935 });
  await page.goto("/play/01-simple");
  await expect(page.locator(".play-area .game-board")).toBeVisible();
  await page.keyboard.press("ArrowUp");
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено");
  await page.getByRole("button", { name: "Порівняти всі алгоритми" }).click();
  await expect(page.locator(".decision-card")).toHaveCount(10);
  const firstCard = page.locator(".decision-card").first();
  await expect(firstCard.locator(".decision-score strong").first()).not.toHaveText("0");
  await page.getByLabel("Порівнювати 1 - BFS", { exact: true }).check();
  await page.getByLabel("Порівнювати 2 - A* ходи", { exact: true }).check();
  await page.getByRole("button", { name: "Порівняти вибрані поруч" }).click();
  const comparison = page.getByRole("region", { name: "Синхронне порівняння" });
  await expect(comparison.locator(".game-board")).toHaveCount(2);
  const restartBtn = comparison.getByRole("button", { name: "На початок" });
  await expect(restartBtn).toBeDisabled();
  await comparison.getByRole("button", { name: "Запустити разом" }).click();
  await expect(comparison.locator("article header span")).toHaveText(["Завершено", "Завершено"]);
  await expect(restartBtn).toBeEnabled();
  await restartBtn.click();
  await expect(restartBtn).toBeDisabled();
  await expect(comparison.locator("article header span").first()).toHaveText("0 / 1");
});
