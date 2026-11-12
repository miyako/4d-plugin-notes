# Ownership regressions

Synthetic-engine ASan/UBSan regressions for 4D native value ownership in
`4DPlugin-Notes.cpp` and `support/4DPlugin-JSON.cpp` (issue #4).

- `fake_engine.cpp` stubs `Call4D` and tracks strings, pictures, objects and
  collections, so leaks and double frees surface as named failures.
- `harness.mm` compiles the actual plugin source with the unchanged SDK
  (`4D Plugin API/4DPluginAPI.c`). Scripting Bridge, `NSAppleScript` and
  `NSWorkspace` are replaced with in-process fakes: no real Notes data is
  read and Notes.app is never launched.

```sh
tests/ownership/run.sh                 # current tree
tests/ownership/run.sh <root> name...  # another tree, listed tests expected to fail
```

`CONVERT PICTURE` (command 1002) is modelled as replacing the picture in place;
a no-op mode covers the case where conversion fails.
