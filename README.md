# Utilitare

## Overview
Utilitare is a small personal project consisting of a collection of utility functions implemented in C. It is organized into several tasks, each addressing a specific functionality – from parsing command-line arguments to directory listing, computing a common directory between two paths, and generating a shortest edit script between files. The following sections describe each task in detail and explain how the core functions work together to achieve the overall functionality.

## Table of Contents

- [Overview](#overview)

- [Tasks](#tasks)

  - [Task 1: Command-line Argument Parser](#task-1-command-line-argument-parser)

  - [Task 2: Directory Listing Utility](#task-2-directory-listing-utility)

  - [Task 3: Relative Directory Computation](#task-3-relative-directory-computation)

  - [Task 4: Shortest Edit Script Generator](#task-4-shortest-edit-script-generator)

## Tasks

### Task 1: Command-line Argument Parser
Task 1 is designed to process and sort the command-line inputs provided to the program. It categorizes the inputs into three types:

1. **Arguments** – Non‑numeric strings or data inputs that are not options or flags.

2. **Options** – Command‑line options that usually come with a numeric value. They can be presented in:

   - **Long Form**: e.g., `--option=value`

   - **Short Form**: e.g., `-o value`

4. **Flags** – Boolean toggles that indicate a state (e.g., activated with `-f` or `--flag`).

These functions work together as follows:

- The core function `sort_arg` iterates through each command-line argument. It uses helper functions such as `is_long` and `is_short` to determine whether an argument is in long or short form.

- Upon identifying a potential option, the `opt` function is used to process the argument. It detects if the numeric value is provided inline (using the `=` sign) or as the next argument. Depending on its findings, `opt` then calls `add_opt` to record the option along with its value.

- When encountering generic arguments (which are neither options nor flags), the function `add_arg` is invoked to add these arguments into a linked list.

- For flags, the function `add_flag` is used. It assigns a default value (typically toggling the flag on) and stores the flag accordingly.

- In cases where bundled short flags are encountered (e.g., `-abc`), the `mult_arg` function splits and records each individual flag.

- Finally, functions such as `find_arg` and `find_f_otp` retrieve and print the arguments, options, and flags respectively, while ensuring that memory is properly freed with `free_list`.

- **Task 1 Functions:**
  - `sort_arg()` – Sorts command‑line arguments into flags, options, and generic arguments.

  - `is_long()`, `is_short()` – Check whether an argument is in long or short form.

  - `opt()` – Processes options that include numeric values.

  - `add_arg()`, `add_opt()`, `add_flag()` – Create and insert new nodes in the respective linked lists.

  - `mult_arg()`, `find_f_otp()`, `find_arg()`, `free_list()` – Handle processing and memory management of input arguments.

### Task 2: Directory Listing Utility
Task 2 implements a directory listing feature. It non‑recursively scans the contents of a directory and prints a hierarchical list of files and subdirectories including their sizes. This is achieved with:

- **getopt** for command-line option processing (e.g., `--recursive`, `--human-readable`).

- **dirent** functions (`opendir`, `readdir`, `closedir`) to iterate over directory entries.

- **sys/stat** (`stat`, `lstat`) to get file metadata, such as file size.

- **Task 2 Functions:**

  - `push()`, `pop_node()` – Manage a dynamic stack of directory paths.

  - `sortdirs()`, `sortfiles()` – Sort directory and file entries alphabetically.

  - `printStack()` – Traverse and print the directory tree with proper indentation.

  - `freestack()`, `freelist()` – Free dynamic memory used by the stack and file lists.

The functions work in tandem as follows:

- The `util_ls` function begins by setting up a directory stack using a custom linked list. It then opens the initial directory with `opendir` and iterates over each entry.

- For each entry, it constructs the full path using `snprintf` to safely format the string.

- The file’s metadata is retrieved using `lstat`; if the entry is a directory (and not a symbolic link), it is pushed onto the stack for later recursive processing.

- After processing entries of the current directory, `printStack` is called to traverse the linked list and output the directory tree structure with proper indentation based on depth.

- Finally, helper functions such as `sortdirs` and `sortfiles` sort the directory and file names alphabetically before printing.

### Task 3: Relative Directory Computation
Task 3 computes the longest common directory between two paths. These functions work in unison to tokenize path strings and then compare tokens to find the common base.

- **Task 3 Functions:**
  - `createroot()`, `adoptchild()` – Construct the directory tree structure.

  - `dfs_stack()` – Perform a DFS on the directory tree to generate a relative path.

  - `free_tree()` – Free the allocated memory for the directory tree.

  - Utilizes string handling functions such as `strdup`, `strtok_r`, `snprintf`, and `strcat` to manage paths safely.

The core functions here include:

- **createroot and adoptchild:**  

  These functions build a directory tree. `createroot` creates a new node for a directory; `adoptchild` adds subdirectories as children of a given node.

- **dfs_stack:**  

  The `dfs_stack` function performs a depth-first search (DFS) on the directory tree to locate a specified target directory. It uses custom stack structures to keep track of the current search path. When the target is found, the relative path is reconstructed from the stack contents. Dynamic memory management is used here with `malloc` and `realloc` for efficient stack resizing.

### Task 4: Shortest Edit Script Generator

Task 4 generates a shortest edit script that details the differences between two files. Its functions integrate file I/O, string processing, and an external edit distance algorithm.

- **Task 4 Functions:**
  - `shortest_edit_script()` – Compare two files line-by-line, logging differences and generating an edit script.

  - Relies on `getline`, `strcmp`, and `fprintf` for input/output and logging.

  - Calls an external `dijkstra` function to compute the edit distance.

Key operations include:
- **File Reading Using getline:**  

  The `getline` function is used to read lines of variable lengths safely from both the reference and the comparison files.
  
- **Line Comparison and Logging:**  

  The function employs `strcmp` to compare each pair of lines from the two files. When differences are detected, it logs detailed information using `fprintf`.
  
- **Edit Distance Calculation:**  

  The external `dijkstra` function is invoked to compute the minimal series of edit operations (insertions, deletions, or substitutions) necessary to transform one line to the other. While the implementation of `dijkstra` is not shown, it is integrated seamlessly.
