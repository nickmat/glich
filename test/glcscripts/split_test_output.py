# split_test_output.py

import tkinter as tk

root = tk.Tk()
root.withdraw()
text = root.clipboard_get()
root.destroy()

if not text.strip().startswith("GlichTest"):
    print("Clipboard does not contain GlichTest output.")
    exit(1)

expected, actual = [], []
current = None

for line in text.splitlines(keepends=True):
    if line.strip() == "Expected:":
        current = expected
    elif line.strip() == "Output:":
        current = actual
    elif current is not None:
        current.append(line)

open("expected.txt", "w", encoding="utf-8").writelines(expected)
open("actual.txt", "w", encoding="utf-8").writelines(actual)
print(f"Done. Expected: {len(expected)} lines, Output: {len(actual)} lines.")
