import { test, expect } from "@playwright/test";
const ids = [
  "01-simple",
  "02-microban",
  "03-microban-3",
  "04-small",
  "05-corridor",
];
test("menu, distinct catalog and all level routes load without page errors", async ({
  page,
}) => {
  const errors: string[] = [];
  page.on("pageerror", (e) => errors.push(e.message));
  await page.goto("/");
  await expect(page.locator(".home-menu a")).toHaveCount(4);
  await expect(page.locator(".entrance")).toHaveCount(0);
  await page.screenshot({
    path: "test-results/home-desktop.png",
    fullPage: true,
  });
  await page.locator(".main-choice").click();
  await expect(page.locator(".level-item")).toHaveCount(6);
  await page.screenshot({
    path: "test-results/catalog-desktop.png",
    fullPage: true,
  });
  for (const id of ids) {
    await page.goto(`/play/${id}`);
    await expect(page.getByRole("img", { name: /Поле:/ })).toBeVisible();
  }
  expect(errors).toEqual([]);
});
test("AI (Groq) applies only a verified plan and reports when AI fails", async ({
  page,
}) => {
  await page.goto("/ai");
  await page.getByLabel("AI API key (Groq або Google)").fill("test-api-key");
  await expect(page.getByRole("main")).toContainText("Groq");
  await page.getByLabel("Модель Groq (швидкі, free tier)").selectOption("openai/gpt-oss-120b");
  await page.getByRole("button", { name: "Зберегти для цієї вкладки" }).click();
  await expect(page.getByRole("status").filter({ hasText: "Ключ" })).toContainText("Ключ готовий");
  const verifiedPlan = await (
    await page.request.post("/api/game", {
      data: {
        levelId: "05-corridor",
        history: "",
        mode: "solve",
        algorithm: "astar-pushes",
      },
    })
  ).json();
  let aiSucceeds = true;
  let transientFailures = 2;
  let aiRequests = 0;
  await page.route("**/api/ai", async (route) => {
    if (route.request().method() === "GET") {
      await route.fallback();
      return;
    }
    aiRequests++;
    const request = await route.request().postDataJSON();
    expect(request.model).toBe("openai/gpt-oss-120b");
    expect(request.levelId).toBe("05-corridor");
    expect(request.apiKey).toBe("test-api-key");
    if (aiSucceeds && transientFailures-- > 0) {
      await route.fulfill({
        status: 503,
        json: { error: "AI зараз перевантажений." },
      });
      return;
    }
    await route.fulfill({
      json: aiSucceeds
        ? {
            ...verifiedPlan,
            algorithm: "gemini",
            status: "Solved",
            validated: true,
            trace: [],
            remoteMs: 12,
            explanation: "AI побудував перевірений маршрут.",
            aiSession: {
              model: "openai/gpt-oss-120b",
              prompt: "Solve this Sokoban position.\n#####",
              response: '{"moves":"UDLR","note":"search()"}',
            },
          }
        : {
            ...verifiedPlan,
            algorithm: "gemini",
            status: "InternalError",
            moves: "",
            pushes: 0,
            validated: false,
            trace: [],
            totalMs: verifiedPlan.totalMs + 1,
            remoteMs: 13,
            explanation:
              "AI не повернув маршрут у форматі UDLR. ШІ не зміг розв’язати цю позицію.",
            aiSession: {
              model: "openai/gpt-oss-120b",
              prompt: "Solve this Sokoban position.\n#####",
              response: '{"error":"no solution"}',
            },
          },
    });
  });
  await page.goto("/play/05-corridor");
  await page.getByLabel("Алгоритм").selectOption("gemini");
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  const decision = page.locator(".decision-card").filter({ hasText: "Відповідь AI" });
  await expect(decision).toContainText("AI побудував перевірений маршрут");
  expect(aiRequests).toBe(3);
  await expect(decision).not.toContainText("Пошук A*");
  await decision.getByText("Переглянути сесію AI").click();
  await expect(decision).toContainText("openai/gpt-oss-120b");
  await expect(decision).toContainText("search()");
  await decision.getByRole("button", { name: "Застосувати" }).click();
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено", {
    timeout: 15000,
  });
  await page.getByRole("button", { name: "Спочатку", exact: true }).click();
  aiSucceeds = false;
  await decision.getByRole("button", { name: "Перерахувати" }).click();
  await expect(decision).toContainText("ШІ не впорався");
  await expect(decision).toContainText("Результат не можна застосувати");
  await expect(decision).toContainText("no solution");
  await expect(decision.getByRole("button", { name: "Застосувати" })).toHaveCount(0);
});
test("manual moves, keyboard, undo, redo, restart and win", async ({
  page,
}) => {
  await page.goto("/play/01-simple");
  await expect(page.locator(".game-board")).toBeVisible();
  await page.keyboard.press("ArrowDown");
  await expect(page.locator(".message")).toContainText("заблоковано");
  await expect(page.getByTestId("moves")).toHaveText("0");
  await page.keyboard.press("ArrowLeft");
  await expect(page.getByTestId("moves")).toHaveText("1");
  await page
    .getByRole("button", { name: "Скасувати", exact: false })
    .first()
    .click();
  await expect(page.getByTestId("moves")).toHaveText("0");
  await page
    .getByRole("button", { name: "Повторити", exact: false })
    .first()
    .click();
  await expect(page.getByTestId("moves")).toHaveText("1");
  await page.getByRole("button", { name: "Спочатку", exact: true }).click();
  await expect(page.getByTestId("moves")).toHaveText("0");
  await page.keyboard.press("ArrowUp");
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено");
  await expect(page.getByTestId("pushes")).toHaveText("1");
});
test("comparison from a moved state, replay pause, step, switching solver and completion", async ({
  page,
}) => {
  await page.setViewportSize({ width: 930, height: 1247 });
  await page.goto("/play/02-microban?debug=1");
  await expect(page.locator(".game-board")).toBeVisible();
  await page.getByRole("button", { name: "Вліво", exact: true }).click();
  await expect(page.getByTestId("moves")).toHaveText("1");
  await page.getByRole("button", { name: "Порівняти всі алгоритми" }).click();
  await expect(
    page.getByRole("button", { name: "Застосувати", exact: true }),
  ).toHaveCount(3);
  await expect(
    page.getByRole("button", { name: "Застосувати", exact: true }).first(),
  ).toBeEnabled();
  await expect(page.locator(".decision-card")).toHaveCount(3);
  await page
    .locator(".decision-card")
    .first()
    .getByRole("button", { name: "Дебаг", exact: true })
    .click();
  await expect(page.locator(".search-debugger")).toBeVisible();
  await expect(page.locator(".debug-game-board")).toBeVisible();
  await expect(page.locator(".debug-phases button")).toHaveCount(3);
  await page.getByRole("button", { name: "Пауза", exact: true }).click();
  const initialSearchState = await page.locator(".debug-game-board").evaluate(
    (board) =>
      Array.from(board.querySelectorAll<HTMLElement>(".actor"))
        .map((actor) => actor.style.transform)
        .join("|"),
  );
  await page.getByRole("button", { name: "Наступний крок дебагу" }).click();
  await page.getByRole("button", { name: "Наступний крок дебагу" }).click();
  expect(
    await page.locator(".debug-game-board").evaluate((board) =>
      Array.from(board.querySelectorAll<HTMLElement>(".actor"))
        .map((actor) => actor.style.transform)
        .join("|"),
    ),
  ).not.toBe(initialSearchState);
  await expect(page.locator(".debug-selected-state")).toContainText("FIFO");
  await page.screenshot({
    path: "test-results/algorithm-debugger-search.png",
    fullPage: true,
  });
  await page.getByRole("button", { name: /02.*Маршрут/ }).click();
  await page.getByRole("button", { name: "Наступний крок дебагу" }).click();
  await expect(page.locator(".board-marker")).not.toHaveCount(0);
  await page.screenshot({
    path: "test-results/algorithm-debugger-route.png",
    fullPage: true,
  });
  await page.getByRole("button", { name: "Продовжити", exact: true }).click();
  await expect(page.locator(".debug-console-top strong")).toHaveText("2 / 6");
  await page.getByRole("button", { name: "Пауза", exact: true }).click();
  await page.getByRole("button", { name: /03.*Рух/ }).click();
  await expect(page.getByRole("button", { name: "Закрити", exact: true })).toBeVisible();
  await page.screenshot({ path: "test-results/algorithm-debugger.png", fullPage: true });
  await page.emulateMedia({ colorScheme: "dark" });
  await page.screenshot({
    path: "test-results/algorithm-debugger-dark.png",
    fullPage: true,
  });
  await page.emulateMedia({ colorScheme: "light" });
  await page.setViewportSize({ width: 390, height: 844 });
  expect(
    await page.evaluate(
      () => document.documentElement.scrollWidth <= window.innerWidth,
    ),
  ).toBe(true);
  await page.screenshot({
    path: "test-results/algorithm-debugger-mobile.png",
    fullPage: true,
  });
  await page.setViewportSize({ width: 930, height: 1247 });
  await page.getByRole("button", { name: "Закрити", exact: true }).click();
  await page.screenshot({
    path: "test-results/game-comparison.png",
    fullPage: true,
  });
  await page
    .getByRole("button", { name: "Застосувати", exact: true })
    .first()
    .click();
  await page.getByRole("button", { name: "Пауза", exact: true }).click();
  await page
    .getByRole("button", { name: "Наступний крок", exact: true })
    .click();
  await expect(page.getByTestId("moves")).toHaveText("2");
  await page
    .getByRole("button", { name: "Попередній крок", exact: true })
    .click();
  await expect(page.getByTestId("moves")).toHaveText("1");
  await page
    .getByRole("button", { name: "Застосувати", exact: true })
    .last()
    .click();
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено", { timeout: 15000 });
  await page.getByRole("button", { name: "Спочатку", exact: true }).click();
  await expect(page.locator(".decision-card")).toHaveCount(3);
  const firstDecision = page.locator(".decision-card").first();
  await Promise.all([
    page.waitForResponse((response) => response.url().includes("/api/game")),
    firstDecision.getByRole("button", { name: "Перерахувати" }).click(),
  ]);
  await expect(page.locator(".decision-card")).toHaveCount(3);
  await firstDecision.getByRole("button", { name: "Видалити" }).click();
  await expect(page.locator(".decision-card")).toHaveCount(2);
  await page.reload();
  await expect(page.locator(".decision-card")).toHaveCount(2);
  await page.screenshot({ path: "test-results/game-won.png", fullPage: true });
});
test("single solver decision persists while the player keeps moving", async ({
  page,
}) => {
  await page.goto("/play/02-microban");
  await expect(page.locator(".game-board")).toBeVisible();
  await expect(page.getByRole("button", { name: "Підказка" })).toHaveCount(0);
  await page.getByRole("button", { name: "Обчислити вибраний" }).click();
  await expect(page.locator(".decision-card")).toHaveCount(1);
  await expect(page.getByRole("button", { name: "Дебаг" })).toHaveCount(0);
  await expect(page.locator(".search-debugger")).toHaveCount(0);
  await expect(page.getByTestId("moves")).toHaveText("0");
  await page.getByRole("button", { name: "Вліво", exact: true }).click();
  await expect(page.getByTestId("moves")).toHaveText("1");
  await expect(page.locator(".decision-card")).toHaveCount(1);
});
test("all teaching routes, next/back/reset, play/pause and history page", async ({
  page,
}) => {
  for (const id of [
    "bfs",
    "astar",
    "reach",
    "reverse",
    "hungarian",
    "deadlocks",
    "hash",
    "replay",
    "generator",
    "parser",
  ]) {
    await page.goto(id === "bfs" ? "/about" : `/about/${id}`);
    await expect(page.locator(".code-line.active")).toHaveCount(1);
    expect(await page.locator(".demo-progress button").count()).toBeGreaterThan(1);
    const start = await page.locator(".demo-bottom p").textContent();
    await page
      .getByRole("button", { name: "Наступний крок пояснення" })
      .click();
    expect(await page.locator(".demo-bottom p").textContent()).not.toBe(start);
    await page
      .getByRole("button", { name: "Попередній крок пояснення" })
      .click();
    await expect(page.locator(".demo-bottom p")).toHaveText(start!);
  }
  await page.goto("/about/hungarian");
  await expect(page.locator(".demo-progress button")).toHaveCount(6);
  await page.getByLabel("Крок 4").click();
  await expect(page.locator(".step-count")).toHaveText("4 / 6");
  await page.getByLabel("Темп").selectOption("1800");
  await page.getByRole("button", { name: "Відтворити", exact: true }).click();
  await page.getByRole("button", { name: "Пауза", exact: true }).click();
  await page.getByRole("button", { name: "Спочатку", exact: true }).click();
  await expect(page.locator(".step-count")).toHaveText("1 / 6");
  await page.waitForTimeout(300);
  await page.screenshot({
    path: "test-results/algorithms-desktop.png",
    fullPage: true,
  });
  await page.goto("/about/deadlocks");
  await page.locator(".demo-progress button").last().click();
  await page.waitForTimeout(650);
  await page.screenshot({ path: "test-results/algorithm-field.png", fullPage: true });
  await page.goto("/game-info");
  await expect(page.locator(".legend-item")).toHaveCount(6);
  expect((await page.goto("/play/missing"))?.status()).toBe(404);
  expect((await page.goto("/about/missing"))?.status()).toBe(404);
});
test("create a seeded custom level, open it and solve it", async ({ page }) => {
  await page.goto("/levels/create");
  await page.getByLabel("Ширина").fill("8");
  await page.getByLabel("Висота").fill("8");
  await page.getByLabel("Ящики й цілі").fill("2");
  await page.getByLabel("Внутрішні стіни").fill("4");
  await page.getByLabel("Seed, необов’язково").fill("12345");
  await page.getByRole("button", { name: "Згенерувати рівень" }).click();
  await expect(page.locator(".generator-result.ready")).toBeVisible({ timeout: 30000 });
  await expect(page.locator(".generated-summary")).toContainText("4");
  await page.waitForTimeout(400);
  await page.screenshot({ path: "test-results/custom-generator.png", fullPage: true });
  await page.getByRole("button", { name: "Грати на цій карті" }).click();
  await expect(page).toHaveURL(/\/play\/custom$/);
  await expect(page.getByRole("img", { name: /Поле:/ })).toBeVisible();
  await expect(page.getByRole("heading", { name: "Власний рівень" })).toBeVisible();
  await page.getByRole("button", { name: "Шлях генератора" }).click();
  await expect(page.locator(".message")).toContainText("перевірене під час генерації");
  await expect(page.getByTestId("level-status")).toHaveText("Рівень завершено", { timeout: 20000 });
  await page.reload();
  await expect(page.getByRole("button", { name: "Відтворити збережене" })).toBeVisible();
});
test("a 100 by 100 custom level uses the available board and camera space", async ({ page }) => {
  await page.setViewportSize({ width: 1593, height: 1247 });
  await page.goto("/levels/create");
  const generated = await page.evaluate(async () => {
    const response = await fetch("/api/generate", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ width: 100, height: 100, boxes: 3, walls: 20, seed: "42" }),
    });
    return response.json();
  });
  await page.evaluate((level) => {
    sessionStorage.setItem("sokoban.custom-level.v1", JSON.stringify(level));
  }, generated);
  await page.goto("/play/custom");
  await expect(page.getByRole("img", { name: /Поле:/ })).toBeVisible();
  await expect(page.locator(".camera-label")).toContainText("Камера 17 × 17");
  const inlineBounds = await page.evaluate(() => {
    const stage = document.querySelector(".board-stage")!.getBoundingClientRect();
    const board = document.querySelector(".game-board")!.getBoundingClientRect();
    return {
      stageTop: stage.top,
      stageBottom: stage.bottom,
      stageWidth: stage.width,
      boardTop: board.top,
      boardBottom: board.bottom,
      boardWidth: board.width,
    };
  });
  expect(inlineBounds.boardTop).toBeGreaterThanOrEqual(inlineBounds.stageTop);
  expect(inlineBounds.boardBottom).toBeLessThanOrEqual(inlineBounds.stageBottom);
  expect(inlineBounds.boardWidth / inlineBounds.stageWidth).toBeGreaterThan(0.9);
  await page.screenshot({ path: "test-results/custom-large-inline.png", fullPage: true });
  await page.getByRole("button", { name: "Повний екран" }).click();
  await expect(page.getByRole("dialog", { name: "Повноекранна карта рівня" })).toBeVisible();
  const fullScreenBounds = await page.evaluate(() => {
    const board = document.querySelector(".map-expanded .game-board")!.getBoundingClientRect();
    const controls = document.querySelector(".camera-controls")!.getBoundingClientRect();
    return {
      boardLeft: board.left,
      boardRight: board.right,
      boardWidth: board.width,
      controlsLeft: controls.left,
      controlsRight: controls.right,
      controlsWidth: controls.width,
      viewportWidth: window.innerWidth,
    };
  });
  expect(fullScreenBounds.boardLeft).toBeLessThanOrEqual(20);
  expect(fullScreenBounds.boardWidth).toBeGreaterThan(1100);
  expect(fullScreenBounds.boardRight).toBeLessThan(fullScreenBounds.controlsLeft);
  expect(fullScreenBounds.controlsWidth).toBeLessThanOrEqual(260);
  expect(fullScreenBounds.controlsRight).toBeLessThanOrEqual(fullScreenBounds.viewportWidth);
  await expect(page.getByLabel("Стежити за гравцем")).toBeChecked();
  const before = await page.locator(".camera-label").textContent();
  const board = page.locator(".game-board");
  const bounds = await board.boundingBox();
  expect(bounds).not.toBeNull();
  await page.mouse.move(bounds!.x + bounds!.width / 2, bounds!.y + bounds!.height / 2);
  await page.mouse.down();
  await page.mouse.move(bounds!.x + bounds!.width / 2 - 90, bounds!.y + bounds!.height / 2);
  await page.mouse.up();
  await expect(page.getByLabel("Стежити за гравцем")).not.toBeChecked();
  expect(await page.locator(".camera-label").textContent()).not.toBe(before);
  await page.getByLabel("Стежити за гравцем").check();
  await page.keyboard.down("Shift");
  await page.keyboard.press("ArrowRight");
  await page.keyboard.up("Shift");
  await expect(page.getByLabel("Стежити за гравцем")).not.toBeChecked();
  await page.getByLabel("Поле огляду камери").fill("25");
  await expect(page.locator(".camera-label")).toContainText("Камера 25 × 25");
  // Move the camera well away from the player: the actor must not leak into page chrome.
  const playerColumn = Number((await page.locator(".map-expanded .game-board").getAttribute("aria-label"))!.match(/стовпець (\d+)/)![1]);
  const panAway = playerColumn > 50 ? "Камера вліво" : "Камера вправо";
  for (let step = 0; step < 22; step++) {
    await page.getByRole("button", { name: panAway, exact: true }).click();
  }
  await expect(page.locator(".map-expanded .actor.player")).toHaveCount(0);
  await page.getByLabel("Стежити за гравцем").check();
  await expect(page.locator(".map-expanded .actor.player")).toHaveCount(1);
  const playerContained = await page.locator(".map-expanded .game-board").evaluate(board => {
    const field = board.getBoundingClientRect();
    const player = board.querySelector(".actor.player")!.getBoundingClientRect();
    return player.left >= field.left && player.right <= field.right && player.top >= field.top && player.bottom <= field.bottom;
  });
  expect(playerContained).toBe(true);
  await page.evaluate(() => (document.activeElement as HTMLElement)?.blur());
  await page.screenshot({ path: "test-results/custom-fullscreen-camera.png" });
  await page.emulateMedia({ colorScheme: "dark" });
  await page.screenshot({ path: "test-results/custom-fullscreen-camera-dark.png" });
  await page.emulateMedia({ colorScheme: "light" });
  await page.getByRole("button", { name: "Закрити карту" }).click();
  await page.setViewportSize({ width: 961, height: 1247 });
  const compactBounds = await page.evaluate(() => {
    const stage = document.querySelector(".board-stage")!.getBoundingClientRect();
    const board = document.querySelector(".game-board")!.getBoundingClientRect();
    return { stageTop: stage.top, stageBottom: stage.bottom, boardTop: board.top, boardBottom: board.bottom };
  });
  expect(compactBounds.boardTop).toBeGreaterThanOrEqual(compactBounds.stageTop);
  expect(compactBounds.boardBottom).toBeLessThanOrEqual(compactBounds.stageBottom);
  await page.setViewportSize({ width: 390, height: 844 });
  await page.getByRole("button", { name: "Повний екран" }).click();
  const mobileBounds = await page.evaluate(() => {
    const board = document.querySelector(".map-expanded .game-board")!.getBoundingClientRect();
    const controls = document.querySelector(".camera-controls")!.getBoundingClientRect();
    return {
      boardLeft: board.left,
      boardRight: board.right,
      controlsLeft: controls.left,
      controlsRight: controls.right,
      controlsBottom: controls.bottom,
      viewportWidth: window.innerWidth,
      viewportHeight: window.innerHeight,
    };
  });
  expect(mobileBounds.boardLeft).toBeGreaterThanOrEqual(0);
  expect(mobileBounds.boardRight).toBeLessThanOrEqual(mobileBounds.viewportWidth);
  expect(mobileBounds.controlsLeft).toBeGreaterThanOrEqual(0);
  expect(mobileBounds.controlsRight).toBeLessThanOrEqual(mobileBounds.viewportWidth);
  expect(mobileBounds.controlsBottom).toBeLessThanOrEqual(mobileBounds.viewportHeight);
  await page.screenshot({ path: "test-results/custom-fullscreen-camera-mobile.png" });
  await page.getByRole("button", { name: "Закрити карту" }).click();
});
test("mobile routes fit viewport; reduced motion and dark mode", async ({
  page,
}) => {
  await page.setViewportSize({ width: 390, height: 844 });
  for (const route of [
    "/",
    "/levels",
    "/play/05-corridor",
    "/about/hungarian",
    "/game-info",
  ]) {
    await page.goto(route);
    if (route.includes("/play"))
      await expect(page.locator(".game-board")).toBeVisible();
    expect(
      await page.evaluate(
        () => document.documentElement.scrollWidth <= window.innerWidth,
      ),
    ).toBe(true);
    await page.screenshot({
      path: `test-results/mobile-${route.replaceAll("/", "-") || "home"}.png`,
      fullPage: true,
    });
  }
  await page.emulateMedia({ colorScheme: "dark", reducedMotion: "reduce" });
  await page.goto("/");
  await expect(page.locator(".entrance")).toHaveCount(0);
  expect(
    await page
      .locator("body")
      .evaluate((e) => getComputedStyle(e).backgroundColor),
  ).toBe("rgb(32, 35, 43)");
  await page.screenshot({
    path: "test-results/home-dark-mobile.png",
    fullPage: true,
  });
});
test("API rejects malformed parameters and confirms a current-state solution", async ({
  request,
}) => {
  expect(
    (
      await request.post("/api/game", { data: { levelId: "../private" } })
    ).status(),
  ).toBe(400);
  expect(
    (
      await request.post("/api/ai", {
        data: { levelId: "01-simple", history: "" },
      })
    ).status(),
  ).toBeGreaterThanOrEqual(400);
  expect(
    (
      await request.post("/api/game", {
        data: "{broken",
        headers: { "Content-Type": "application/json" },
      })
    ).status(),
  ).toBe(400);
  const base = {
    levelId: "01-simple",
    history: "L",
    mode: "solve",
    algorithm: "astar-moves",
  };
  const result = await (await request.post("/api/game", { data: base })).json();
  expect(result.validated).toBe(true);
  const replay = await (
    await request.post("/api/game", {
      data: { ...base, mode: "state", history: "L" + result.moves },
    })
  ).json();
  expect(replay.won).toBe(true);
  expect(
    (
      await request.post("/api/generate", {
        data: { width: 5, height: 5, boxes: 1, walls: 1, seed: "1" },
      })
    ).status(),
  ).toBe(400);
});
