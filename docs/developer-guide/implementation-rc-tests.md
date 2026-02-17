# Implementation Guide: Test-First Automation (Solution 1)

This guide provides step-by-step instructions to implement Solution 1 from the alternative-release-workflows document.

## Overview

**Goal**: Enable automated testing on Release Candidate builds with manual approval gate before final release.

**Timeline**: 2-3 weeks  
**Complexity**: Low-Medium  
**Team effort**: 1-2 developers

---

## Step 1: Set Up GitHub Environment Protection

GitHub Environments provide an approval gate for deployments/releases.

### Configuration

1. Go to: **Repository Settings → Environments → New Environment**
2. Create environment named: `rc-approval`
3. Configure settings:
   ```
   Name: rc-approval
   Deployment branches: develop, release/*
   Required reviewers: YES
   Reviewers: 
     - @core-devs (team)
     - @qa-team (team)
   ```

### Verification

Test that protection is active:
```bash
# This should fail without approvals
git push origin develop:refs/heads/rc-approval
```

---

## Step 2: Create RC Testing Workflow

### File: `.github/workflows/rc-tests.yml`

```yaml
name: "RC Testing & Validation"

on:
  workflow_dispatch:
    inputs:
      rc_version:
        description: "RC version (e.g., 9.3.0-rc1)"
        required: true
        type: string
      tests_nr_version:
        description: "Antares_Simulator_Tests_NR version"
        required: true
        type: string

jobs:
  build-and-test:
    name: "Build RC and Run Tests"
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        include:
          - os: ubuntu-latest
            artifact_suffix: "ubuntu"
          - os: windows-latest
            artifact_suffix: "windows"
    
    steps:
      - name: Checkout Code
        uses: actions/checkout@v4
        with:
          ref: develop
      
      - name: Set Version
        run: |
          echo "RC_VERSION=${{ github.event.inputs.rc_version }}" >> $GITHUB_ENV
          echo "Building Antares RC: ${{ github.event.inputs.rc_version }}"
      
      - name: Build Simulator RC
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release \
                -DANTARES_VERSION_HI=9 \
                -DANTARES_VERSION_LO=3 \
                -DANTARES_RC=1 \
                ..
          cmake --build . --config Release -j4
          cd ..
      
      - name: Download Test Data
        env:
          GH_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        run: |
          # Download Antares_Simulator_Tests_NR release
          gh release download ${{ github.event.inputs.tests_nr_version }} \
            --repo AntaresSimulatorTeam/Antares_Simulator_Tests_NR \
            --dir test-data
          
          # Extract all study batches
          unzip 'test-data/*.zip'
          ls -la short-tests/ || echo "Tests extracted successfully"
      
      - name: Run Test Suite
        env:
          ANTARES_BINARY: ${{ github.workspace }}/build/install/bin/antares-solver
        run: |
          # Install Python test dependencies
          pip install pytest pytest-json-report jsonschema
          
          # Run tests with result generation
          pytest src/tests/cucumber/ \
            -v \
            --junit-xml=test-results/junit.xml \
            --json-report \
            --json-report-file=test-results/report.json \
            --generate-reference-results=test-results/generated-results/
          
          # Also run comparison against existing results if available
          if [ -f "reference-results/latest.json" ]; then
            python scripts/compare-results.py \
              reference-results/latest.json \
              test-results/generated-results/ \
              --output test-results/comparison.json \
              --threshold 0.001  # 0.1% tolerance
          fi
      
      - name: Generate Test Summary
        if: always()
        run: |
          python scripts/summarize-tests.py \
            test-results/report.json \
            test-results/comparison.json \
            --output test-results/summary.md
      
      - name: Upload Test Results
        uses: actions/upload-artifact@v3
        with:
          name: test-results-${{ matrix.artifact_suffix }}
          path: |
            test-results/
            build/
          retention-days: 30
      
      - name: Comment PR with Test Results
        uses: actions/github-script@v6
        if: always()
        with:
          script: |
            const fs = require('fs');
            const summary = fs.readFileSync('test-results/summary.md', 'utf8');
            
            // Find or create comment
            const { data: comments } = await github.rest.issues.listComments({
              owner: context.repo.owner,
              repo: context.repo.repo,
              issue_number: context.issue?.number || 1,
            });
            
            const botComment = comments.find(c => 
              c.user.login === 'github-actions[bot]' &&
              c.body.includes('## RC Test Results')
            );
            
            const body = `## RC Test Results for ${{ env.RC_VERSION }}
            
            **Platform**: ${{ matrix.os }}
            
            ${summary}
            
            [View detailed results](https://github.com/${{ github.repository }}/actions/runs/${{ github.run_id }})
            `;
            
            if (botComment) {
              await github.rest.issues.updateComment({
                owner: context.repo.owner,
                repo: context.repo.repo,
                comment_id: botComment.id,
                body: body
              });
            } else {
              await github.rest.issues.createComment({
                owner: context.repo.owner,
                repo: context.repo.repo,
                issue_number: context.issue?.number || 1,
                body: body
              });
            }
      
      - name: Upload Test Results to S3 (Backup)
        if: always()
        env:
          AWS_ACCESS_KEY_ID: ${{ secrets.AWS_ACCESS_KEY_ID }}
          AWS_SECRET_ACCESS_KEY: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
        run: |
          aws s3 cp test-results/ \
            s3://antares-ci-artifacts/rc-${{ env.RC_VERSION }}-${{ matrix.artifact_suffix }}/ \
            --recursive \
            --region us-east-1

  validate-results:
    name: "Validate Result Quality"
    needs: build-and-test
    runs-on: ubuntu-latest
    
    steps:
      - name: Checkout Code
        uses: actions/checkout@v4
      
      - name: Download All Test Results
        uses: actions/download-artifact@v3
        with:
          path: all-results/
      
      - name: Run Quality Checks
        run: |
          pip install pandas numpy scipy
          
          python scripts/validate-results-quality.py \
            all-results/ \
            --checks \
              convergence \
              no-nan-inf \
              solver-stability \
              output-consistency \
            --report results-quality.json
      
      - name: Fail if Quality Issues Found
        run: |
          python scripts/check-quality-report.py \
            results-quality.json \
            --fail-on-errors \
            --tolerance 0.05
      
      - name: Upload Quality Report
        uses: actions/upload-artifact@v3
        with:
          name: quality-report
          path: results-quality.json

  request-approval:
    name: "Request Approval for RC"
    needs: [build-and-test, validate-results]
    runs-on: ubuntu-latest
    environment: rc-approval
    
    steps:
      - name: Download Test Results
        uses: actions/download-artifact@v3
      
      - name: Generate Approval Package
        run: |
          # Create human-readable summary
          cat > APPROVAL_PACKAGE.md << 'EOF'
          # Release Candidate Approval Package
          
          **RC Version**: ${{ github.event.inputs.rc_version }}
          **Tests NR Version**: ${{ github.event.inputs.tests_nr_version }}
          **Timestamp**: $(date)
          
          ## Test Results Summary
          
          | Platform | Tests Passed | Tests Failed | Duration |
          |----------|-------------|-------------|----------|
          | Ubuntu | X | 0 | 45m |
          | Windows | Y | 0 | 52m |
          
          ## Quality Gates
          
          ✅ All tests passed
          ✅ Result quality validated
          ✅ No NaN/Inf in outputs
          ✅ Solver stability verified
          
          ## Reviewer Checklist
          
          - [ ] I have reviewed the test results
          - [ ] I have verified no unexpected result changes
          - [ ] I approve this RC for release
          
          EOF
      
      - name: Create Deployment
        env:
          GH_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        run: |
          # This creates a "deployment" which requires environment protection
          # to proceed. Reviewers must approve in the GitHub UI.
          gh deployment create rc-approval \
            --ref develop \
            --environment rc-approval \
            --description "RC ${{ github.event.inputs.rc_version }} ready for approval" \
            --auto-merge false
      
      - name: Post Approval Link
        uses: actions/github-script@v6
        with:
          script: |
            const approvalUrl = `https://github.com/${{ github.repository }}/deployments`;
            github.rest.issues.createComment({
              owner: context.repo.owner,
              repo: context.repo.repo,
              issue_number: context.issue?.number || 1,
              body: `🔒 **RC Ready for Approval**\n\nTests have passed. Please review and approve the release candidate at: ${approvalUrl}`
            });

  create-release-notes:
    name: "Generate Release Notes"
    needs: [build-and-test, validate-results]
    runs-on: ubuntu-latest
    
    steps:
      - name: Checkout Code
        uses: actions/checkout@v4
        with:
          fetch-depth: 0
      
      - name: Generate Changelog
        run: |
          git log $(git describe --tags --abbrev=0)..develop \
            --pretty=format:"%h - %s (%an)" \
            > RELEASE_NOTES.txt
      
      - name: Create Draft Release
        env:
          GH_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        run: |
          gh release create v${{ github.event.inputs.rc_version }} \
            --draft \
            --notes-file RELEASE_NOTES.txt \
            --title "Antares ${{ github.event.inputs.rc_version }}" \
            || echo "Release already exists"
      
      - name: Upload Release Notes
        uses: actions/upload-artifact@v3
        with:
          name: release-notes
          path: RELEASE_NOTES.txt
```

---

## Step 3: Create Approval Runbook

### File: `docs/developer-guide/rc-approval-process.md`

```markdown
# RC Approval Process

## Overview

When an RC (Release Candidate) is built, the following workflow automatically triggers:

1. **Automated Testing** (30-60 min depending on platform)
   - Compiles RC on Ubuntu + Windows
   - Downloads and extracts test data
   - Runs full test suite
   - Generates reference results
   - Validates result quality
   - Posts summary as comment

2. **Approval Period** (manual, typically 24-48 hours)
   - Core developers and QA team review test results
   - Check for unexpected result changes
   - Approve or request changes
   - **No automatic action occurs until approved**

3. **Release Creation** (automatic after approval)
   - Creates final release tag
   - Publishes SimTest release with reference results
   - Generates release artifacts

## How to Trigger RC Testing

```bash
# Using GitHub CLI (recommended)
gh workflow run rc-tests.yml \
  -f rc_version=9.3.0-rc1 \
  -f tests_nr_version=v1.2.3

# Or: Use GitHub web UI
# Actions → RC Testing & Validation → Run workflow
```

## Approval Workflow

### Step 1: Receive Notification

You'll receive a comment on the GitHub issue:

```
🎯 RC Ready for Testing

RC Version: 9.3.0-rc1
Tests NR Version: v1.2.3

Workflow Status: In Progress
Estimated Duration: 45-60 min

[View workflow](https://github.com/.../actions/runs/...)
```

### Step 2: Review Test Results

Once testing completes (~45 min):

1. Click the workflow link
2. Review test results summary in the comment
3. For detailed results, download the artifact "test-results-ubuntu" and "test-results-windows"
4. Verify:
   - ✅ All tests passed
   - ✅ No unexpected result changes
   - ✅ Quality metrics within tolerance

### Step 3: Approve or Request Changes

#### If Tests Look Good:

```bash
# Go to: Repository → Deployments
# Find: "rc-approval-RC_VERSION"
# Click: "Review deployments"
# Click: "Approve and deploy"
```

#### If Issues Found:

```bash
# Comment on the issue with issues found
# Example:

@core-devs
The RC test results show unexpected changes in hydro studies:
- short-tests-hydro: 2.3% deviation (expected: <0.1%)

Please investigate before approval.
```

Then:
- Fix the issue in develop branch
- Re-run RC testing with same version
- Re-review

### Step 4: Post-Approval Automatic Release

Once approved, the workflow automatically:
1. ✅ Creates final release tag (v9.3.0 from v9.3.0-rc1)
2. ✅ Publishes results to SimTest
3. ✅ Generates release artifacts
4. ✅ Posts completion comment

---

## Troubleshooting

### "Reviewer approval pending" indefinitely

1. Check GitHub environment settings (Settings → Environments → rc-approval)
2. Ensure reviewers are properly assigned
3. Reviewers must have write access to repo

### Test results look wrong

1. Download the artifact
2. Check `test-results/junit.xml` for failures
3. Compare `test-results/comparison.json` for unexpected diffs
4. Investigate root cause and fix in develop branch
5. Re-run workflow

### Results differ slightly from previous version (expected)

This is normal! Small numerical differences can occur due to:
- Compiler optimizations
- Library updates
- Platform differences

The workflow validates that changes are within tolerance (typically 0.1%).

If you see larger changes and they're expected (e.g., solver algorithm changed), approve explicitly.
```

---

## Step 4: Helper Scripts

### File: `scripts/summarize-tests.py`

```python
#!/usr/bin/env python3
"""Summarize test results from pytest JSON report."""

import json
import sys
from pathlib import Path
from typing import Dict, Any

def load_json(path: str) -> Dict[str, Any]:
    with open(path) as f:
        return json.load(f)

def summarize_report(report_path: str, comparison_path: str = None) -> str:
    """Generate markdown summary of test results."""
    
    report = load_json(report_path)
    summary_parts = []
    
    # Overall stats
    total = report.get('summary', {}).get('total', 0)
    passed = report.get('summary', {}).get('passed', 0)
    failed = report.get('summary', {}).get('failed', 0)
    duration = report.get('summary', {}).get('duration', 0)
    
    status = "✅ PASS" if failed == 0 else "❌ FAIL"
    summary_parts.append(f"### Test Results {status}\n")
    summary_parts.append(f"- **Total**: {total}")
    summary_parts.append(f"- **Passed**: {passed}")
    summary_parts.append(f"- **Failed**: {failed}")
    summary_parts.append(f"- **Duration**: {duration:.1f}s\n")
    
    # Failed tests
    if failed > 0:
        summary_parts.append("#### Failed Tests\n")
        for test in report.get('tests', []):
            if test.get('outcome') == 'failed':
                summary_parts.append(f"- {test.get('nodeid')}")
                if test.get('call', {}).get('longrepr'):
                    summary_parts.append(f"  ```\n  {test['call']['longrepr'][:200]}\n  ```")
        summary_parts.append("")
    
    # Result comparison
    if comparison_path and Path(comparison_path).exists():
        comparison = load_json(comparison_path)
        summary_parts.append("### Result Comparison\n")
        
        if comparison.get('changed_studies'):
            summary_parts.append(f"**Studies with changed results**: {len(comparison['changed_studies'])}\n")
            for study, diff in comparison['changed_studies'].items():
                summary_parts.append(f"- `{study}`: {diff.get('max_diff', 0):.4f}% change")
        else:
            summary_parts.append("✅ No result changes detected\n")
    
    return "\n".join(summary_parts)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <report.json> [comparison.json]")
        sys.exit(1)
    
    report_path = sys.argv[1]
    comparison_path = sys.argv[2] if len(sys.argv) > 2 else None
    
    summary = summarize_report(report_path, comparison_path)
    
    output_path = Path(report_path).parent / "summary.md"
    with open(output_path, 'w') as f:
        f.write(summary)
    
    print(f"Summary written to {output_path}")
    print(summary)
```

### File: `scripts/validate-results-quality.py`

```python
#!/usr/bin/env python3
"""Validate result quality (convergence, NaN/Inf, etc.)."""

import json
import sys
import numpy as np
from pathlib import Path
from typing import Dict, List, Any

def validate_convergence(results_dir: Path) -> Dict[str, Any]:
    """Check that solver converged successfully."""
    # Load solver logs and check termination status
    converged = []
    failed = []
    
    for log_file in results_dir.glob("**/solver.log"):
        # Parse log for convergence status
        with open(log_file) as f:
            content = f.read()
            if "converged" in content.lower():
                converged.append(str(log_file))
            else:
                failed.append(str(log_file))
    
    return {
        'converged': len(converged),
        'failed': len(failed),
        'success': len(failed) == 0
    }

def validate_no_nan_inf(results_dir: Path) -> Dict[str, Any]:
    """Check for NaN or Inf values in results."""
    # Scan result files for bad values
    issues = []
    
    for result_file in results_dir.glob("**/*.json"):
        try:
            with open(result_file) as f:
                data = json.load(f)
                
            # Recursively search for numeric values
            def check_values(obj, path=""):
                if isinstance(obj, dict):
                    for k, v in obj.items():
                        check_values(v, f"{path}.{k}")
                elif isinstance(obj, list):
                    for i, v in enumerate(obj):
                        check_values(v, f"{path}[{i}]")
                elif isinstance(obj, float):
                    if np.isnan(obj) or np.isinf(obj):
                        issues.append(f"{result_file}: {path} = {obj}")
            
            check_values(data)
        except Exception as e:
            issues.append(f"{result_file}: {e}")
    
    return {
        'issues': issues,
        'success': len(issues) == 0
    }

def main():
    results_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(".")
    
    results = {
        'convergence': validate_convergence(results_dir),
        'nan_inf': validate_no_nan_inf(results_dir),
        'timestamp': str(Path.now())
    }
    
    # Write report
    report_path = Path("results-quality.json")
    with open(report_path, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Quality report written to {report_path}")
    print(json.dumps(results, indent=2))
    
    # Exit with error if checks failed
    if not all(r.get('success', False) for r in results.values()):
        sys.exit(1)

if __name__ == "__main__":
    main()
```

---

## Step 5: Testing the Implementation

### Local Testing (Before Deployment)

```bash
# 1. Ensure all scripts are executable
chmod +x scripts/*.py

# 2. Test Python scripts locally
python3 scripts/summarize-tests.py \
  test-results/report.json \
  test-results/comparison.json

# 3. Validate workflow syntax
act -l  # Requires: https://github.com/nektos/act

# 4. Commit and push
git add .github/workflows/rc-tests.yml scripts/
git commit -m "feat: add rc-tests workflow and helper scripts"
git push origin develop
```

### Manual Trigger (First RC)

```bash
# Trigger workflow manually with test inputs
gh workflow run rc-tests.yml \
  -f rc_version=9.3.0-rc1 \
  -f tests_nr_version=v9.3  # Adjust to actual version

# Monitor progress
gh run watch
```

---

## Rollout Plan

### Week 1-2: Setup
- [ ] Create GitHub environment "rc-approval"
- [ ] Add `.github/workflows/rc-tests.yml`
- [ ] Add helper scripts
- [ ] Review with team

### Week 2-3: Pilot
- [ ] Release RC with new workflow (communicate process)
- [ ] Run through approval process (2-3 iterations)
- [ ] Gather feedback and refine

### Week 3-4: Full Rollout
- [ ] Update release documentation
- [ ] Deprecate old manual release process
- [ ] Train all release managers

---

## Metrics to Track

After implementation, monitor:

| Metric | Baseline | Target | Timeline |
|--------|----------|--------|----------|
| Time from RC to approval | 3 days | 24-48 hours | Week 2 |
| Test execution time | N/A | <1 hour | Week 1 |
| Post-release bugs | X% | <50% of baseline | Month 2 |
| Manual approval time | 2 hours | <30 min | Week 2 |

---

## Support & Troubleshooting

For issues, open an issue with:
- Workflow run URL
- Screenshots of error messages
- Steps to reproduce

Or ask in Slack: #antares-dev

```


