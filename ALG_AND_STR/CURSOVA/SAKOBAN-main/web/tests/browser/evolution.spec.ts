import { test, expect } from "@playwright/test";

test("web-only ACO and GA results expose an interactive observer", async ({ page }) => {
  test.setTimeout(90000);
  for (const algorithm of ["aco", "genetic"]) {
    await page.goto("/play/01-simple");
    await page.getByLabel("Алгоритм").selectOption(algorithm);
    await page.getByRole("button", { name: "Обчислити вибраний" }).click();
    const card = page.locator(".decision-card").filter({ hasText: algorithm === "aco" ? "Прототип 2" : "Прототип 3" });
    await expect(card).toContainText("Розв’язано", { timeout: 30000 });
    await card.locator(".evolution-observer > summary").click();
    await expect(card.locator(".evolution-viewer")).toBeVisible();
    await expect(card.locator(".gene-strip span").first()).toBeVisible();
    await card.getByRole("button", { name: "Показати переможця" }).click();
    await expect(card.locator(".evolution-summary")).toContainText("Ціль досягнута");
    await expect(card.locator(".evolution-stage .game-board")).toBeVisible();
    await card.screenshot({ path: `test-results/${algorithm}-observer.png` });
    await card.getByRole("button", { name: "Відтворити" }).click();
    await expect(card.locator(".evolution-playback")).toContainText(/1 \/|2 \//, { timeout: 5000 });
    expect(await card.evaluate(element => element.scrollWidth <= element.clientWidth + 1)).toBeTruthy();
    expect(await card.locator(".evolution-stage .game-board").evaluate(board => {
      const bounds = board.getBoundingClientRect();
      return Array.from(board.querySelectorAll(".board-marker")).every(marker => {
        const box = marker.getBoundingClientRect();
        return box.left >= bounds.left - 1 && box.right <= bounds.right + 1 &&
          box.top >= bounds.top - 1 && box.bottom <= bounds.bottom + 1;
      });
    })).toBeTruthy();
  }
});
