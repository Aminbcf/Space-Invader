#!/usr/bin/env python3
import os
import sys

def to_lf(path):
    with open(path, 'rb') as f:
        content = f.read()
    
    # Check if file has CRLF
    if b'\r\n' in content:
        new_content = content.replace(b'\r\n', b'\n')
        with open(path, 'wb') as f:
            f.write(new_content)
        print(f"Fixed: {path}")
    else:
        # print(f"Skipped (already LF): {path}")
        pass

def main():
    extensions = {'.c', '.h', '.cpp', '.hpp', '.sh', 'Makefile', '.mk', '.txt', '.md'}
    specific_files = {'Makefile'}
    
    print("Starting CRLF -> LF conversion...")
    count = 0
    for root, dirs, files in os.walk("."):
        if ".git" in root or "build" in root:
            continue
            
        for name in files:
            ext = os.path.splitext(name)[1]
            if ext in extensions or name in specific_files:
                path = os.path.join(root, name)
                try:
                    to_lf(path)
                    count += 1
                except Exception as e:
                    print(f"Error processing {path}: {e}")
    
    print(f"Done. Processed {count} files.")

if __name__ == "__main__":
    main()
