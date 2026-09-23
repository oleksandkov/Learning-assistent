import { test, expect } from "@playwright/test";

test("web-only population methods expose an interactive observer", async ({ page }) => {
  test.setTimeout(90000);
  await page.setViewportSize({ width: 755, height: 1247 });
  const methods = {
    aco: { card: "Прототип 2", count: 28, plural: "мурах" },
    genetic: { card: "Прототип 3", count: 36, plural: "хромосом" },
    gravity: { card: "Прототип 6", count: 28, plural: "потоків" },
    cannibal: { card: "Прототип 7", count: 64, plural: "ботів" },
  } as const;
  for (const [algorithm, method] of Object.entries(methods)) {
    await page.goto("/play/01-simple");
    await page.getByLabel("Алгоритм").selectOption(algorithm);
    await page.getByRole("button", { name: "Обчислити вибраний" }).click();
    const card = page.locator(".decision-card").filter({ hasText: method.card });
    await expect(card).toContainText("Розв’язано", { timeout: 30000 });
    await card.locator(".evolution-observer > summary").click();
    await expect(card.locator(".evolution-viewer")).toBeVisible();
    if (algorithm === "gravity") {
      await expect(card.locator(".method-stage-shell.gravity .board-cell-hint").first()).toBeVisible();
      await expect(card.locator(".gravity-live-readout")).toContainText("h =");
    }
    if (algorithm === "cannibal") {
      await expect(card.locator(".method-stage-shell.cannibal .cannibal-live-dna span")).toHaveCount(3);
      await expect(card.locator(".cannibal-live-readout.lineage")).toContainText("нова альтернатива");
    }
    await expect(card.locator(".gene-strip span").first()).toBeVisible();
    await card.getByRole("button", { name: "Показати переможця" }).click();
    await expect(card.locator(".evolution-summary")).toContainText("Ціль досягнута");
    await expect(card.locator(".evolution-stage .game-board")).toBeVisible();
    await card.screenshot({ path: `test-results/${algorithm}-observer.png` });
    await card.getByRole("button", { name: "Відтворити" }).click();
    await expect(card.locator(".evolution-playback")).toContainText(/1 \/|2 \//, { timeout: 5000 });
    const { count, plural } = method;
    await card.getByRole("button", { name: `Усі ${count} ${plural}` }).click();
    await expect(card.locator(".generation-roster-list button")).toHaveCount(count);
    await expect(card.locator(".board-marker")).toHaveCount(count);
    await expect(card.locator(".board-marker.leader")).toHaveCount(1);
    await card.getByRole("button", { name: `Запустити всіх ${plural}` }).click();
    await expect(card.locator(".evolution-board-panel .evolution-panel-heading")).toContainText("Крок 1 /", { timeout: 5000 });
    await card.screenshot({ path: `test-results/${algorithm}-generation.png` });
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
