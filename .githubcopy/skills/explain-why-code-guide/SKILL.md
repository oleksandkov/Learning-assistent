---
name: explain-why-code-guide
description: Use for deep explanation when the user asks deep, detailed, in-depth, full, step-by-step, or comprehensive understanding of code, functions, methods, classes, APIs, or concepts, with cited web sources and bilingual output.
argument-hint: Provide language, code snippet or file path, your level, and what exactly you want explained (feature, function, method, class, bug, or concept).
user-invocable: true
disable-model-invocation: false
---

# Explain Why Code Guide (Deep)

Deep student-friendly explanation workflow for code, functions, methods, and concepts.

## When To Use

Use this skill when the user asks with phrases like:

- deep explanation
- explain in detail
- in-depth explanation
- full explanation
- explain step by step
- explain it simple (in this workflow, simple words with deep coverage)
- teach this concept deeply
- show detailed examples
- why this works internally

Use it for any programming language, with priority support for Java, C++, Python, and JavaScript.

## Objectives

1. Explain the code/concept clearly and deeply.
2. Show practical examples and expected results.
3. Explain usage in current and similar scenarios.
4. Search reliable internet sources (official docs, tutorial platforms, technical references, Q and A databases) and return curated learning resources with direct URLs.
5. Provide explanation in both English and Ukrainian in the same chat response.

## Required Behavior

1. Start by identifying the learner level and goal.
2. If details are missing, ask targeted questions using VS Code Ask Questions.
3. Read local project context first when code is provided.
4. Search the internet for trusted sources relevant to the exact function/concept.
5. Prioritize official documentation and high-quality educational tutorials.
6. Include direct source URLs alongside the explanation and resource list.
7. If a tutorial or documentation page contains a relevant example, include a source-attributed example in the response.
8. If no source example is available, explicitly state that and provide an original example.
9. Provide both explanation and actionable next steps for students.
10. If the prompt contains both simple and deep intent, keep the explanation easy to read but still complete and deep.

## Source Quality Policy

Prioritize sources in this order:

1. Official documentation
2. Language/reference databases (for example: Java API docs, cppreference, Python docs, MDN)
3. Reputable tutorial platforms and university/course materials
4. Community Q and A/discussion sources for edge cases

Avoid low-quality, outdated, or unclear sources.

## Explanation Procedure

1. Identify what the code unit is: function, method, class, API, pattern, or full program flow.
2. Break down logic step by step.
3. Explain input, output, side effects, and constraints.
4. Explain why this approach works.
5. Show at least one minimal example and one practical example.
6. Explain common mistakes and how to debug them.
7. Show at least one similar scenario where the same concept applies.
8. Add source links near key claims and examples when external material is used.

## Required Output Format

Return two language blocks in this order:

1. English version
2. Ukrainian version

Inside each language block, return sections in this order:

1. Core idea
2. Step-by-step explanation
3. Minimal example
4. Practical example
5. How to use in current codebase
6. Common mistakes and debugging tips
7. Similar use cases
8. Learning resources

## Trigger Boundary

- If the request is only quick, short, or fast understanding, use the fast explanation skill instead.
- If the request asks for internal logic, tradeoffs, edge cases, or full understanding, use this deep skill.

## Learning Resources Section Rules

For each resource include:

- Title
- Link (full URL, mandatory)
- Resource type (official docs, API reference, tutorial, video, article, Q and A)
- Best for (beginner/intermediate/advanced)
- Why it is useful
- What exactly it explains (function/method/concept)
- Example from source (if available)
- Example URL (if different from the main resource URL)

Provide 3 to 8 resources when possible.

## Citation and Example Rules

- Whenever external information is used, attach at least one direct URL in the explanation section.
- Prefer examples adapted from official docs or trusted tutorials, and clearly label the source.
- If the original example is modified, mention that it is adapted and keep the source URL.
- Do not present web-derived examples without attribution.

## Student-Centered Style

- Use simple language first, then technical terms.
- Keep explanations precise but supportive.
- Prefer concrete examples over abstract theory.
- Connect theory to real coding tasks.
- End with a short practice task.
- Keep English and Ukrainian versions semantically aligned.
