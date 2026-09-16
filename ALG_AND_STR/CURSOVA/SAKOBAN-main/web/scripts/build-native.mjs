import { existsSync, mkdirSync, readdirSync, statSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const root = fileURLToPath(new URL("../../", import.meta.url));
const output = path.join(
  root,
  "bindings/web/build",
  process.platform === "win32" ? "sokoban_web.exe" : "sokoban_web",
);
const inputs = ["src/core", "src/solvers"].flatMap((dir) =>
  readdirSync(path.join(root, dir))
    .filter((f) => /\.(cpp|hpp)$/.test(f))
    .map((f) => path.join(root, dir, f)),
);
inputs.push(path.join(root, "bindings/web/native.cpp"));
if (
  existsSync(output) &&
  inputs.every((f) => statSync(f).mtimeMs < statSync(output).mtimeMs)
) {
  console.log("C++ web bridge is up to date.");
  process.exit(0);
}
const compiler =
  process.env.CXX ||
  ["g++", "C:/msys64/ucrt64/bin/g++.exe", "C:/msys64/mingw64/bin/g++.exe"].find(
    (c) => spawnSync(c, ["--version"], { windowsHide: true }).status === 0,
  );
if (!compiler)
  throw new Error(
    "C++20 compiler not found. Install GCC (MSYS2 on Windows) or set CXX to g++.",
  );
mkdirSync(path.dirname(output), { recursive: true });
const args = [
  "-std=c++20",
  "-O2",
  "-Wall",
  "-Wextra",
  "-I",
  path.join(root, "src"),
  ...inputs.filter((f) => f.endsWith(".cpp")),
  "-o",
  output,
];
if (process.platform === "win32")
  args.push("-static", "-static-libgcc", "-static-libstdc++");
const built = spawnSync(compiler, args, {
  cwd: root,
  stdio: "inherit",
  windowsHide: true,
});
if (built.error) throw built.error;
process.exit(built.status ?? 1);
