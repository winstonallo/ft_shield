import sys

if __name__ == "__main__":
    s: str = sys.argv[1]
    for c in s:
        print(f"'{c}' ^ 0x42,", end="")
