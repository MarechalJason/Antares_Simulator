# Git Workflow

## Branch Naming (CI requirements)

Use these patterns for CI to run:
- feature/* or features/*
- fix/*
- release/*
- issue-*
- doc/*
- tests/*

Enable git hooks:

```bash
git config core.hooksPath .githooks
```
