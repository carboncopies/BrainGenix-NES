import os
import re
import argparse
import sys

# No filename sanitization needed for single output file

#script to scan C++ files in a repository for API definitions

def extract_apis_to_single_file(repo_path, output_file):
    """
    Finds C++ files, extracts unique API names from AddRoute calls,
    and writes them to a single output file, overwriting previous content.

    Args:
        repo_path (str): The root path of the C++ repository.
        output_file (str): The path to the text file where results will be saved.
    """

    # Regular expression to find the AddRoute line and capture the API name.
    # **Removed the start anchor (^) to find AddRoute anywhere on the line.**
    # Now matches AddRoute( followed by optional space, quote, captured name, quote, comma, anything, );
    pattern = re.compile(r'AddRoute\(\s*"([^"]+)"\s*,.*?\);') # <-- MODIFIED LINE

    # Use a set to automatically store only unique API names
    api_names_set = set()
    files_processed = 0
    matches_found = 0 # Total occurrences found

    print(f"Scanning directory: {repo_path}")

    # Walk through the directory recursively
    for dirpath, _, filenames in os.walk(repo_path):
        for filename in filenames:
            # Process only files ending with .cpp
            if filename.lower().endswith(".cpp"):
                filepath = os.path.join(dirpath, filename)
                files_processed += 1
                try:
                    # Open and read the file line by line
                    # Use utf-8 encoding and ignore errors for robustness
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as infile:
                        for line_num, line in enumerate(infile, 1):
                            # Search for the pattern anywhere in the current line
                            match = pattern.search(line) # pattern.search finds the first match anywhere
                            if match:
                                # Extract the captured group (the API name)
                                api_name = match.group(1)

                                if not api_name: # Skip if the captured name is empty
                                    continue

                                matches_found += 1
                                # Add the found API name to the set (duplicates are ignored)
                                api_names_set.add(api_name)

                except Exception as read_e:
                    print(f"Error processing file {filepath}: {read_e}", file=sys.stderr)

    print(f"\nProcessed {files_processed} C++ files.")
    print(f"Found {matches_found} total AddRoute calls matching the pattern.")
    unique_apis_count = len(api_names_set)
    print(f"Found {unique_apis_count} unique API definitions.")

    if unique_apis_count == 0:
        print("No API definitions matching the pattern were found.")
        return

    # --- Write unique names to the single output file ---
    try:
        # Open the output file in write mode ('w')
        # This will create the file if it doesn't exist, or
        # **overwrite its content if it does exist.**
        with open(output_file, 'w', encoding='utf-8') as outfile:
            # Sort the unique names alphabetically for consistent output
            sorted_names = sorted(list(api_names_set))
            # Write each unique name on a new line
            for name in sorted_names:
                outfile.write(name + '\n')
        print(f"Successfully wrote {len(sorted_names)} unique API definitions to {output_file}")
    except Exception as write_e:
        print(f"Error writing to output file {output_file}: {write_e}", file=sys.stderr)

'''
# --- Script Execution ---
if __name__ == "__main__":
    # Set up argument parser for command-line usage
    parser = argparse.ArgumentParser(
        description="Extract unique API definitions from AddRoute calls in C++ files into a single file."
    )
    parser.add_argument(
        "repo_path",
        help="Path to the root directory of the C++ repository."
    )
    parser.add_argument(
        "-o", "--output",
        default="API_defenitions.txt", # Default filename spelling corrected later if needed
        help="Name of the output text file (default: API_defenitions.txt)"
    )

    args = parser.parse_args()

    # Validate repository path
    if not os.path.isdir(args.repo_path):
        print(f"Error: Repository path '{args.repo_path}' not found or is not a directory.", file=sys.stderr)
    else:
        # Run the main function with the correct arguments
        extract_apis_to_single_file(args.repo_path, args.output)
        # Print the output file path
        print(f"Output file: {os.path.abspath(args.output)}")

'''
# commenting out the script execution part to avoid running it directly


extract_apis_to_single_file(
    repo_path="../../", # Use current directory for testing
    output_file="API_defenitions.txt" # Default output file name
)

def find_undocumented_apis(all_apis_file, docs_file, output_file):
    """
    Compares APIs listed in all_apis_file with those documented in docs_file
    based on the ' - Name: `<api_name>`' pattern in Markdown, and writes
    the undocumented APIs to output_file.

    Args:
        all_apis_file (str): Path to the file containing all extracted API names (one per line).
        docs_file (str): Path to the Markdown documentation file.
        output_file (str): Path to the file where the list of undocumented APIs will be written.
    """

    # --- 1. Read all API names extracted from the source code ---
    all_apis_set = set()
    try:
        print(f"Reading all extracted APIs from: {all_apis_file}")
        with open(all_apis_file, 'r', encoding='utf-8') as f_all:
            for line in f_all:
                # Strip whitespace and ignore empty lines
                api_name = line.strip()
                if api_name:
                    all_apis_set.add(api_name)
        print(f"-> Found {len(all_apis_set)} unique APIs in '{all_apis_file}'.")
        if not all_apis_set:
             print("Warning: The list of all APIs is empty. No comparison possible.", file=sys.stderr)
             # Optionally create empty output and exit if desired
             # with open(output_file, 'w', encoding='utf-8') as f_out: pass
             # return

    except FileNotFoundError:
        print(f"Error: Input file '{all_apis_file}' not found. Cannot proceed.", file=sys.stderr)
        return # Stop execution if this crucial file is missing
    except Exception as e:
        print(f"Error reading {all_apis_file}: {e}", file=sys.stderr)
        return # Stop execution on error reading this file

    # --- 2. Read documented API names from the Markdown docs file ---
    documented_apis_set = set()
    # **MODIFIED Regex** to find lines like " - Name: `API/Name/Here`" and capture the API name.
    # It looks for start of line (^), optional whitespace (\s*), literal '-',
    # one or more spaces (\s+), literal 'Name:', optional whitespace (\s*),
    # literal backtick (`` ` ``), captures non-backtick characters ([^`]+), literal backtick (`` ` ``).
    docs_pattern = re.compile(r'^\s*-\s+Name:\s*`([^`]+)`') # <-- MODIFIED LINE
    try:
        print(f"Reading documented APIs from Markdown file: {docs_file}") # Updated print message
        with open(docs_file, 'r', encoding='utf-8', errors='ignore') as f_docs: # errors='ignore' for robustness
            for line_num, line in enumerate(f_docs, 1):
                match = docs_pattern.search(line)
                if match:
                    # Extract the captured group (API name) and strip potential extra whitespace
                    api_name = match.group(1).strip()
                    if api_name:
                        documented_apis_set.add(api_name)
                    else:
                         print(f"Warning: Found ' - Name: ``' tag without API name in {docs_file} at line {line_num}.", file=sys.stderr)

        print(f"-> Found {len(documented_apis_set)} documented APIs in '{docs_file}'.")

    except FileNotFoundError:
        # If docs file is missing, treat all APIs as undocumented
        print(f"Warning: Documentation file '{docs_file}' not found. Assuming all APIs need documentation.", file=sys.stderr)
        # documented_apis_set remains empty, which is the correct state for comparison
    except Exception as e:
        print(f"Error reading {docs_file}: {e}", file=sys.stderr)
        # Decide whether to proceed or return; let's proceed but warn user
        print("Warning: Proceeding comparison, but documentation data might be incomplete.", file=sys.stderr)


    # --- 3. Find the difference (APIs in all_apis_set but not in documented_apis_set) ---
    undocumented_apis = all_apis_set - documented_apis_set
    count_undocumented = len(undocumented_apis)
    print(f"-> Comparison complete: Found {count_undocumented} APIs potentially needing documentation.")

    # --- 4. Write the difference to the output file ---
    try:
        print(f"Writing undocumented APIs to: {output_file}")
        # Open in write mode ('w') to overwrite the file each time
        with open(output_file, 'w', encoding='utf-8') as f_out:
            if undocumented_apis:
                # Sort the list alphabetically for consistent output
                sorted_undocumented = sorted(list(undocumented_apis))
                for api_name in sorted_undocumented:
                    f_out.write(api_name + '\n')
                print(f"Successfully wrote {len(sorted_undocumented)} API names to '{output_file}'.")
            else:
                # Write nothing if the set is empty, effectively creating an empty file
                print("No APIs requiring documentation were found. Output file is empty or overwritten.")
                pass # Ensures an empty file is created/overwritten

    except Exception as e:
        print(f"Error writing to output file '{output_file}': {e}", file=sys.stderr)

find_undocumented_apis("API_defenitions.txt", "../../Docs/API.md", "api_to_be_documented.txt")



# --- End of Script ---