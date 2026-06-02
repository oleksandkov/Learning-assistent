/** @jsxImportSource @opentui/solid */
import { createSignal, createEffect, createMemo, For, onCleanup } from "solid-js"
import type {
  TuiPlugin,
  TuiPluginApi,
  TuiPluginModule,
  TuiPromptRef,
} from "@opencode-ai/plugin/tui"

// ============================================================================
// Types
// ============================================================================

type Mood =
  | "idle"
  | "walk"
  | "happy"
  | "thinking"
  | "scared"
  | "sleeping"
  | "celebrating"
  | "typing"
  | "reading"
  | "editing"
  | "bash"

type Pixel = " " | "b" | "o" | "w" | "g" | "p" | "y" | "r"
type FrameRow = string
type Frame = FrameRow[]
type Animation = Frame[]

type RGB = { r: number; g: number; b: number }
type Cell = { ch: string; fg: string; bg: string }

const SPRITE_W = 16
const SPRITE_H = 16
const CELL_ROWS = SPRITE_H / 2

// ============================================================================
// Palette
// ============================================================================

const PALETTE: Record<Pixel, RGB | null> = {
  " ": null,
  b: { r: 26, g: 26, b: 26 },
  o: { r: 243, g: 156, b: 42 },
  w: { r: 248, g: 232, b: 216 },
  g: { r: 124, g: 195, b: 106 },
  p: { r: 232, g: 154, b: 168 },
  y: { r: 241, g: 196, b: 15 },
  r: { r: 231, g: 76, b: 60 },
}

const hex = (c: RGB) =>
  "#" + [c.r, c.g, c.b].map((v) => v.toString(16).padStart(2, "0")).join("")

// ============================================================================
// Sprite data — orange tabby cat, 16x16 pixels
//   " " = transparent
//   b   = black outline
//   o   = orange body
//   w   = white belly
//   g   = green eyes
//   p   = pink nose
//   y   = yellow accent
//   r   = red
// ============================================================================

const SPRITES: Record<Mood, Animation> = {
  idle: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boggggoooboooooob",
      ".bggggggobooooooob",
      ".boggggoooboooooob",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbbbbbbbbb....",
    ],
  ],

  walk: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boggggoooboooooob",
      ".bggggggobooooooob",
      ".boggggoooboooooob",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "..bbooooooobboob..",
      "...bbooooobboob...",
    ],
  ],

  happy: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boyyyoobbooooob.",
      ".byyyyobboooooooob",
      ".byyyyobboooooooob",
      ".boyyyoobooooooob.",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbbbbbbbbb....",
    ],
  ],

  thinking: [
    [
      ".......y.........",
      "......yyy........",
      ".....y...y.......",
      ".....y...y.......",
      ".....y.y.........",
      "......y..........",
      "....bbbb....bbbb.",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boggggoooboooooob",
      ".bggggggobooooooob",
      ".boggggoooboooooob",
      ".booooooopooooooob",
    ],
  ],

  scared: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".bobbbbobbobbbbobb",
      ".bobbboobobbbboob",
      ".bobbboobobbbboob",
      ".bobbbbobbobbbbobb",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbbbbbbbbb....",
    ],
  ],

  sleeping: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".bo----obbooooob.",
      ".b-----bbooooooob",
      ".b-----bbooooooob",
      ".bo----oboooooob.",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "..ybooooooobboob..",
      ".yy.bbbbbbbbbb....",
    ],
  ],

  celebrating: [
    [
      "..y..r....y..r...",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".boyyyoobboyyyob",
      ".byyyyobboyyyyob.",
      ".byyyyobboyyyyob.",
      ".boyyyoobboyyyob.",
      ".boooooooboooooob",
      ".booooooopoooooob",
      ".boooooooboooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbbbbbbbbb....",
    ],
  ],

  typing: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooobooooooob",
      ".bggggggoooboooooob",
      ".boggggoooboooooob",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooowwwoob",
      "....bbbbbbwwww....",
    ],
  ],

  reading: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boogggggbgggggob",
      ".boggggggbggggggb",
      ".boggggggbggggggb",
      ".boogggggbgggggob",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbbbbbbbbb....",
    ],
  ],

  editing: [
    [
      "................",
      "....bbbb....bbbb",
      "...boooob...boooob",
      "..booooooobboooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".booooooooobooooob",
      ".boggggoooboooooob",
      ".bggggggobooooooob",
      ".boggggoooboooooob",
      ".booooooopooooooob",
      ".booooooobooooooob",
      ".bbooooooobooooob.",
      "..bbooooooobboob..",
      "...bbooooooobboob.",
      "....bbybbbbbbbbyb.",
    ],
  ],

  bash: [
    [
      "................",
      "..y....b.......b.",
      "yy....bbb....bbb.",
      ".y...boooob..boooob",
      "....booooooobboooob",
      "...booooooooobooooob",
      "...booooooooobooooob",
      "...bobbbbobbobbbbobb",
      "...bobbboobobbbboob",
      "...bobbboobobbbboob",
      "...bobbbbobbobbbbobb",
      "...booooooopooooooob",
      "...bbooooooobooooob.",
      "....bbooooooobboob..",
      ".....bbooooooobboob.",
      "......bbbbbbbbbb....",
    ],
  ],
}

const DEFAULT_MOOD: Mood = "idle"
const RESET_MS = 2500
const TICK_MS = 130
const TYPING_RESET_MS = 1500

// ============================================================================
// Sprite rendering
// ============================================================================

function renderCells(anim: Animation, frameIdx: number): Cell[][] {
  const frame = anim[frameIdx % anim.length]
  const rows: Cell[][] = []
  for (let r = 0; r < CELL_ROWS; r++) {
    const top = frame[r * 2]
    const bot = frame[r * 2 + 1]
    const cells: Cell[] = []
    for (let c = 0; c < SPRITE_W; c++) {
      const tp = PALETTE[top[c] as Pixel]
      const bp = PALETTE[bot[c] as Pixel]
      const tf = tp ? hex(tp) : ""
      const bf = bp ? hex(bp) : ""
      let ch = " "
      let fg = tf
      let bg = ""
      if (!tp && !bp) {
        ch = " "
      } else if (!tp) {
        ch = "▄"
        fg = bf
      } else if (!bp) {
        ch = "▀"
        fg = tf
      } else {
        ch = "▀"
        fg = tf
        bg = bf
      }
      cells.push({ ch, fg, bg })
    }
    rows.push(cells)
  }
  return rows
}

// ============================================================================
// CatSprite component
// ============================================================================

function CatSprite(props: { mood: Mood; frame: number }) {
  const anim = createMemo(() => SPRITES[props.mood] ?? SPRITES[DEFAULT_MOOD])
  const rows = createMemo(() => renderCells(anim(), props.frame))

  return (
    <box flexDirection="column" flexShrink={0}>
      <For each={rows()}>
        {(row) => (
          <text>
            <For each={row}>
              {(cell) =>
                cell.ch === " " ? (
                  <span> </span>
                ) : cell.bg ? (
                  <span style={{ fg: cell.fg, bg: cell.bg }}>{cell.ch}</span>
                ) : (
                  <span style={{ fg: cell.fg }}>{cell.ch}</span>
                )
              }
            </For>
          </text>
        )}
      </For>
    </box>
  )
}

// ============================================================================
// Store
// ============================================================================

type Store = ReturnType<typeof createStore>

function createStore() {
  const [mood, setMoodSignal] = createSignal<Mood>(DEFAULT_MOOD)
  const [frame, setFrameSignal] = createSignal(0)

  let resetTimer: ReturnType<typeof setTimeout> | null = null

  const setMood = (next: Mood, autoResetMs?: number) => {
    setMoodSignal(next)
    if (resetTimer) {
      clearTimeout(resetTimer)
      resetTimer = null
    }
    if (autoResetMs && autoResetMs > 0) {
      resetTimer = setTimeout(() => setMoodSignal(DEFAULT_MOOD), autoResetMs)
    }
  }

  const tick = () => setFrameSignal((f) => (f + 1) % 1_000_000)

  const dispose = () => {
    if (resetTimer) {
      clearTimeout(resetTimer)
      resetTimer = null
    }
  }

  return { mood, setMood, frame, tick, dispose }
}

// ============================================================================
// Prompt wrapper
// ============================================================================

const DEFAULT_PLACEHOLDERS = {
  normal: ["Ask anything…", "Describe a change…", "What should I do?"],
  shell: ["Run a command…"],
}

function PromptWithCat(props: {
  api: TuiPluginApi
  store: Store
  sessionID?: string
  visible?: boolean
  disabled?: boolean
  onSubmit?: () => void
  ref?: (r: TuiPromptRef | undefined) => void
}) {
  const Prompt = props.api.ui.Prompt
  const [refSignal, setRefSignal] = createSignal<TuiPromptRef | undefined>()

  // React to prompt input/focus changes (typing detection).
  createEffect(() => {
    const r = refSignal()
    if (!r) return
    const input = r.current.input
    const focused = r.focused
    if (focused && input.length > 0) {
      props.store.setMood("typing", TYPING_RESET_MS)
    }
  })

  onCleanup(() => setRefSignal(undefined))

  return (
    <Prompt
      sessionID={props.sessionID}
      visible={props.visible}
      disabled={props.disabled}
      onSubmit={props.onSubmit}
      ref={(r) => {
        setRefSignal(() => r)
        props.ref?.(r)
      }}
      placeholders={DEFAULT_PLACEHOLDERS}
      right={
        <box flexDirection="row" flexShrink={0}>
          <CatSprite mood={props.store.mood()} frame={props.store.frame()} />
        </box>
      }
    />
  )
}

// ============================================================================
// Plugin entry
// ============================================================================

const tui: TuiPlugin = async (api, options, meta) => {
  const store = createStore()

  const tickHandle = setInterval(store.tick, TICK_MS)
  api.lifecycle.onDispose(() => {
    clearInterval(tickHandle)
    store.dispose()
  })

  // Transient moods auto-reset to default.
  api.event.on("session.idle", () => store.setMood("happy", RESET_MS))
  api.event.on("session.error", () => store.setMood("scared", RESET_MS))
  api.event.on("session.created", () => store.setMood("happy", RESET_MS))

  api.event.on("session.status", (event) => {
    const status = (event as { properties?: { status?: { type?: string } } })
      .properties?.status
    if (status?.type === "busy") {
      store.setMood("thinking")
    } else {
      store.setMood(DEFAULT_MOOD)
    }
  })

  // Tool events: session.next.tool.{called,success,failed}
  api.event.on("session.next.tool.called", (event) => {
    const tool =
      (event as { properties?: { tool?: string } }).properties?.tool ?? ""
    if (
      tool === "read" ||
      tool === "glob" ||
      tool === "grep" ||
      tool === "list"
    ) {
      store.setMood("reading")
    } else if (
      tool === "edit" ||
      tool === "write" ||
      tool === "patch"
    ) {
      store.setMood("editing")
    } else if (tool === "bash" || tool === "shell") {
      store.setMood("bash")
    } else {
      store.setMood("thinking")
    }
  })

  api.event.on("session.next.tool.success", () => {
    store.setMood("happy", 1200)
  })

  api.event.on("session.next.tool.failed", () => {
    store.setMood("scared", 1800)
  })

  // Register slots that host the prompt with the cat in the right meta row.
  api.slots.register({
    slots: {
      home_prompt(ctx, value) {
        return <PromptWithCat api={api} store={store} ref={value.ref} />
      },

      session_prompt(ctx, value) {
        return (
          <PromptWithCat
            api={api}
            store={store}
            sessionID={value.session_id}
            visible={value.visible}
            disabled={value.disabled}
            onSubmit={value.on_submit}
            ref={value.ref}
          />
        )
      },
    },
  })
}

const plugin: TuiPluginModule & { id: string } = {
  id: "pet.cat",
  tui,
}

export default plugin
