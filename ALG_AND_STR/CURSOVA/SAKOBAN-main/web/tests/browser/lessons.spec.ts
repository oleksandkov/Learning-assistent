import { test, expect } from "@playwright/test";
import { readFile } from "node:fs/promises";
import path from "node:path";
import { LESSONS } from "../../src/lib/lessons";

test("all lessons keep every visual step attached to actual C++ source", async ({ page }) => {
  test.setTimeout(120000);
  const errors: string[] = [];
  page.on("pageerror", error => errors.push(error.message));
  await page.setViewportSize({ width: 745, height: 887 });
  for (const lesson of LESSONS) {
    await page.goto(lesson.id === "bfs" ? "/about" : `/about/${lesson.id}`);
    for (let step = 0; step < lesson.frames.length; step++) {
      const frame = lesson.frames[step];
      await page.getByLabel(`Крок ${step + 1}`, { exact: true }).click();
      await expect(page.locator(".lesson-step-heading h3")).toHaveText(frame.title);
      await expect(page.locator(".algorithm-scene")).toHaveAttribute("data-scene", lesson.id);
      await expect(page.locator(".algorithm-scene")).toHaveAttribute("data-step", String(step));
      const file = frame.source ?? lesson.source;
      const source = (await readFile(path.join(process.cwd(), "..", file), "utf8")).split(/\r?\n/);
      const line = source.findIndex(line => line.includes(frame.anchor));
      expect(line, `${lesson.id}: ${frame.anchor}`).toBeGreaterThanOrEqual(0);
      await expect(page.locator(".source-file")).toHaveText(`${file}:${line + 1}`);
      await expect(page.locator(".code-line.active .code-text")).toHaveText(source[line].trim());
      expect(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth)).toBe(true);
    }
    await page.getByRole("button", { name: "Спочатку", exact: true }).click();
    await expect(page.locator(".step-count")).toHaveText(`1 / ${lesson.frames.length}`);
    await page.setViewportSize({ width: 390, height: 844 });
    expect(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth)).toBe(true);
    await page.setViewportSize({ width: 745, height: 887 });
  }
  expect(errors).toEqual([]);
});

test("algorithm decisions differ and board movement stays continuous", async ({ page }) => {
  await page.goto("/about/astar");
  await page.getByLabel("Крок 3", { exact: true }).click();
  await expect(page.locator(".candidate.chosen .candidate-name")).toHaveText("A");
  await page.getByLabel("Крок 4", { exact: true }).click();
  await expect(page.locator(".candidate.chosen .candidate-name")).toHaveText("B");
  await page.goto("/about/greedy");
  await page.getByLabel("Крок 2", { exact: true }).click();
  await expect(page.locator(".candidate.chosen .candidate-name")).toHaveText("B");
  await page.goto("/about/hungarian");
  await page.getByLabel("Крок 3", { exact: true }).click();
  await expect(page.locator(".cost-matrix td.conflict")).toHaveCount(2);
  await page.getByLabel("Крок 6", { exact: true }).click();
  await expect(page.locator(".cost-matrix td.selected")).toHaveText(["3", "1"]);
  await page.goto("/about/replay");
  const box = page.locator(".lesson-actor.box");
  await box.evaluate(element => element.setAttribute("data-persistent", "yes"));
  await page.getByLabel("Крок 3", { exact: true }).click();
  await expect(box).toHaveAttribute("data-persistent", "yes");
  expect(await box.evaluate(element => getComputedStyle(element).transitionProperty)).toContain("transform");
  await page.getByLabel("Крок 5", { exact: true }).click();
  await expect(box).toHaveClass(/on-goal/);
  await page.emulateMedia({ reducedMotion: "reduce" });
  expect(await box.evaluate(element => parseFloat(getComputedStyle(element).transitionDuration))).toBeLessThan(.01);
});

test("playback stops, resumes, resets, and starts fresh on a new topic", async ({ page }) => {
  await page.goto("/about/idastar");
  await page.clock.install();
  await page.getByLabel("Темп").selectOption("1800");
  await page.getByRole("button", { name: "Відтворити", exact: true }).click();
  await page.clock.runFor(1900);
  await expect(page.locator(".step-count")).toHaveText("2 / 6");
  await page.getByRole("button", { name: "Пауза", exact: true }).click();
  await page.clock.runFor(6000);
  await expect(page.locator(".step-count")).toHaveText("2 / 6");
  await page.getByRole("button", { name: "Попередній крок пояснення" }).click();
  await expect(page.locator(".step-count")).toHaveText("1 / 6");
  await page.getByLabel("Крок 5", { exact: true }).click();
  await page.getByRole("button", { name: "Відтворити", exact: true }).click();
  await page.clock.runFor(1900);
  await expect(page.locator(".step-count")).toHaveText("6 / 6");
  await page.getByRole("button", { name: "Ще раз", exact: true }).click();
  await expect(page.locator(".step-count")).toHaveText("1 / 6");
  await page.getByRole("button", { name: "Спочатку", exact: true }).click();
  await page.clock.runFor(6000);
  await expect(page.locator(".step-count")).toHaveText("1 / 6");
  await page.getByRole("link", { name: "Greedy", exact: true }).click();
  await expect(page.locator(".step-count")).toHaveText("1 / 4");
  await expect(page.getByRole("button", { name: "Відтворити", exact: true })).toBeVisible();
});

test("ACO and GA explanations show their own motion language", async ({ page }) => {
  await page.goto("/about/aco");
  await expect(page.locator(".teaching-ant")).toHaveCount(8);
  const leader = page.locator(".teaching-ant.leader");
  const start = await leader.getAttribute("style");
  await page.getByLabel("Крок 6", { exact: true }).click();
  await expect(leader).not.toHaveAttribute("style", start ?? "");
  await expect(page.locator(".pheromone-legend i")).toHaveAttribute("style", /scaleX\(1\)/);

  await page.goto("/about/genetic");
  await page.getByLabel("Крок 4", { exact: true }).click();
  await expect(page.locator(".child-row")).toHaveClass(/visible/);
  await page.getByLabel("Крок 5", { exact: true }).click();
  await expect(page.locator(".teaching-genes .mutated")).toHaveCount(1);
  expect(await page.locator(".scene-genetic").evaluate(scene => scene.scrollWidth <= scene.clientWidth + 1)).toBeTruthy();
});

test("Drain and Cannibal explanations make force and DNA transfer visible", async ({ page }) => {
  await page.goto("/about/gravity");
  await page.getByLabel("Крок 5", { exact: true }).click();
  await expect(page.locator(".gravity-stage")).toHaveClass(/shaking/);
  await expect(page.locator(".drain-vectors .blocked")).toHaveCount(1);
  expect(await page.locator(".drain-mass").evaluate(element => getComputedStyle(element).transitionProperty)).toContain("transform");

  await page.goto("/about/cannibal");
  await page.getByLabel("Крок 5", { exact: true }).click();
  await expect(page.locator(".cannibal-card.offspring")).toHaveClass(/visible/);
  await expect(page.locator(".dna-transfer")).toHaveClass(/visible/);
  await expect(page.locator(".cannibal-genes.offspring .from-victim")).toHaveCount(4);
  await expect(page.locator(".cannibal-genes.offspring .escape")).toHaveCount(1);
  expect(await page.locator(".cannibal-genes.offspring span").first().evaluate(element => getComputedStyle(element).transitionProperty)).toContain("transform");
  expect(await page.locator(".scene-cannibal").evaluate(scene => scene.scrollWidth <= scene.clientWidth + 1)).toBeTruthy();
});
