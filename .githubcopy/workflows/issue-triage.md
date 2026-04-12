---
on:
  issues:
    types: [opened]
permissions:
  issues: write
---

# Automated Issue Triage

When a new issue is opened, please analyze the title and body text:
1. Automatically apply the appropriate taxonomy labels (e.g., 'bug', 'enhancement', or 'documentation').
2. If the issue appears to be a bug report but the user did not include explicit "Steps to reproduce", leave a polite comment asking them to provide those details so the team can investigate.