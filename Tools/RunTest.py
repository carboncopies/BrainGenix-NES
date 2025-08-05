import subprocess
import time
import signal
import os
import psutil  # You'll need to install this with pip install psutil

def kill_process_tree(pid):
    try:
        parent = psutil.Process(pid)
        children = parent.children(recursive=True)
        for child in children:
            child.kill()
        parent.kill()
    except psutil.NoSuchProcess:
        pass

while True:
    try:
        # Start the command
        process = subprocess.Popen(['./Run.sh'])
        print(f"Started Run.sh with PID {process.pid}")

        # Wait for 10 seconds
        time.sleep(60)

        # Kill the process and its children
        print(f"Killing process tree for PID {process.pid}")
        kill_process_tree(process.pid)

        # Optional: Wait a moment before restarting
        time.sleep(1)
        
    except KeyboardInterrupt:
        print("Script stopped by user")
        break
    except Exception as e:
        print(f"Error occurred: {e}")
        time.sleep(5)  # Wait before retrying in case of error
