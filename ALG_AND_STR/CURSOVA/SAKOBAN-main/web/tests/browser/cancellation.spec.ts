import { test, expect } from "@playwright/test";
test("manual input discards a delayed solver response and cancels busy state", async ({
  page,
}) => {
  await page.goto("/play/02-microban");
  await expect(page.locator(".game-board")).toBeVisible();
  let release!: () => void;
  const held = new Promise<void>((resolve) => {
    release = resolve;
  });
  await page.route("**/api/game", async (route) => {
    if (route.request().postDataJSON().mode !== "solve")
      return route.continue();
    const response = await route.fetch();
    await held;
    await route.fulfill({ response }).catch(() => {});
  });
  await page
    .getByRole("button", { name: "Обчислити вибраний", exact: true })
    .click();
  await expect(
    page.getByRole("button", { name: "Скасувати пошук" }),
  ).toBeVisible();
  await page.keyboard.press("ArrowLeft");
  await expect(page.getByTestId("moves")).toHaveText("1");
  release();
  await expect(page.locator(".results")).toHaveCount(0);
  await expect(
    page.getByRole("button", { name: "Обчислити вибраний", exact: true }),
  ).toBeEnabled();
});
