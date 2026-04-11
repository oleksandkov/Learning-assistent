---
name: explain-fast-code-guide
description: Use for fast explanation when the user says explain, what it do, what does this do, quick explain, short explain, or quick understanding of code, function, method, or concept.
argument-hint: Provide language and code snippet or function name for a short and simple explanation.
user-invocable: true
disable-model-invocation: false
---

# Explain Fast Code Guide

Fast student-friendly explanation workflow for quick understanding.

## When To Use

Use this skill when the user asks with phrases like:

- explain
- what it do
- what does this do
- quick explain
- short explanation
- explain quickly

Use it for any programming language.

## Main Goal

Give very short and clear understanding of what the code does and how to use it now.

## Key Rules

1. English only.
2. Use simple words and short sentences.
3. Keep explanation short and practical.
4. Always include examples.
5. Search trusted web sources and provide URLs only, with no URL commentary text.
6. Prefer quick understanding over deep theory.

## Required Behavior

1. Identify the target element (code block, function, method, class, API call, or concept).
2. If needed, ask one short clarification question using VS Code Ask Questions.
3. Give a quick idea of what it does.
4. Show one minimal example and one practical example.
5. Show expected result/output in simple form.
6. Provide source links as plain URLs only.

## Trigger Boundary

- If the request includes deep, detailed, in-depth, full, or step-by-step analysis, use the deep explanation skill.
- If the request only asks explain or what it does, use this fast skill.

## Required Output Format

1. Quick idea
2. Minimal example
3. Practical example
4. Expected result
5. URLs

## URL Rules

- In the URLs section, include only plain URLs, one per line.
- Do not add titles, labels, or explanations around URLs.
- Provide 2 to 6 URLs when possible.
- Provide the name of the website and the state of the source (official docs, tutorial, Q&A, etc.) in the URL if possible, but only as part of the URL text itself.

## Student Style

- Be clear and friendly.
- Avoid heavy terminology.
- Focus on immediate understanding and use.
