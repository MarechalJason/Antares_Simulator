# Access Control for Agents

Policy: Agents MUST NOT read or parse files under `src/ui` unless a repository maintainer grants explicit, recorded authorization.

Exceptions:
- Maintainer authorization recorded as a PR or issue comment.
- Urgent security or licensing inspections with maintainer approval.

How to request access:
1. Open an issue or PR explaining the need.
2. Wait for a maintainer comment granting access. Include the link when referencing UI files.

Enforcement (practical):
- Exclude `src/ui` from automated searches.
- If a `src/ui` file is returned, ignore it and log: `Access denied: src/ui`.
- If authorized, log: `Access granted: src/ui (see <link>)`.
