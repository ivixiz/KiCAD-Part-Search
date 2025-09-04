#FILE: __main__.py 
import subprocess
import os
from .version import __version__

try:
    import pcbnew
    pcbnew_version = pcbnew.Version()
except Exception:
    print("Could not load `pcbnew`, probably running from virtual environment")
    pcbnew_version = None

def app():
    if pcbnew_version:
        print(f"pcbnew version: {pcbnew_version}")
    print(f"plugin version: {__version__}")

    # Compute path to C++ binary relative to this file
    this_file_dir = os.path.dirname(os.path.abspath(__file__))  # plugin/
    src_dir = os.path.abspath(os.path.join(this_file_dir, '..'))  # src/
    cpp_binary = os.path.join(src_dir, 'main')

    print(f"Resolved C++ binary path: {cpp_binary}")

    if os.path.exists(cpp_binary):
        try:
            result = subprocess.run([cpp_binary], capture_output=True, text=True)
            print("C++ output:\n", result.stdout)
        except Exception as e:
            print("Failed to run C++ binary:", e)
    else:
        print(f"C++ binary not found at {cpp_binary}")

if __name__ == "__main__":
    app()
