import re
import sys


def obfuscate_string(s: str, key=0x42) -> str:
    res: list[str] = []
    for c in s:
        if c == '\n': res.append(f"0x{0x0a ^ key:02x}")
        else: res.append(f"0x{ord(c) ^ key:02x}")
    res.append("0x00")
    return "{" + ", ".join(res) + "}"

def process_file(content: str) -> str:
    def replace_obfuscate(match: re.Match) -> str:
        string_content: str = match.group(1)
        decoded_string: str = eval(f'"{string_content}"')
        return obfuscate_string(decoded_string)
    return re.sub(r'OBFUSCATE\("([^"]+)"\)', replace_obfuscate, content)

if __name__ == "__main__":
    with open(sys.argv[1], 'r') as f:
        content: str = f.read()

    print(process_file(content))
