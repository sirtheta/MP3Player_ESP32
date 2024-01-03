import os

folder_path = "H:/02"

# List all files in the folder
files = os.listdir(folder_path)

# Filter only the mp3 files
mp3_files = [file for file in files if file.endswith(".mp3")]

# Rename the files
for mp3_file in mp3_files:
    # Split the file name into parts
    parts = mp3_file.split(" ")
    
    # Extract the number part and format it to three digits
    number_part = "{:03d}".format(int(parts[0]))
    
    # Join the parts back together with the formatted number
    new_name = f"{number_part} {' '.join(parts[1:])}"
    
    # Create the full paths
    old_path = os.path.join(folder_path, mp3_file)
    new_path = os.path.join(folder_path, new_name)
    
    # Rename the file
    os.rename(old_path, new_path)
    print(f"Renamed: {mp3_file} to {new_name}")