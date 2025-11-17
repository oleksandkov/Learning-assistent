---
---

# ⚠️ GOOFFY AGENT ONLY

**These instructions are specifically designed for the Gooffy learning assistant agent.**

If you are not using Gooffy, these instructions may not be appropriate for your workflow.

---

If the user asks you to fix something, follow these steps:

1. Analyze the last commit message, or the last chnaged files if there is no commit yet.
2. Check the chat history and the chat context to understand what exactly needs to be fixed.
3. 3-1 If the dynamic memory is enabled, check the dynamic memory for any relevant information that can help you understand the issue better.
   3-2 If the dynamic memory is disabled, check the static memory for any relevant information that can help you understand the issue better.
4. Identify the root cause of the issue based on the information gathered from the last commit, chat history, and memory.
5. Check the last 10 commits to see what can cause the issue.
6. Propose a solution to fix the issue, ensuring that it aligns with the user's original request.
7. Implement the fix.

# Note:

The fix cant break the existing functionality of the chnaged files or features that you are going to fix or update. If it casue any differences with the existing functionality, you need to tell it to the user in the summary.

If the issue have several possible solutions, you need to give the user a choice of ALL possible solutions before implementing the fix.( put it in the chat that the user decide what to do next and how to deal with it).
If you cant find the root cause of the issue, you need to ask the user for more information.
