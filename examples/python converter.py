# nx-reader logs to SysBot.py syntax converter example

# Open log file and create new python file
with open("nxreader.log", "r") as logs, open("buttons.py", "w") as f:
    for line in logs:

        # Check if press or touch
        if "Press" in line:

            # Button decipher
            button = (line.split(": ", 2)[2]).replace("\n", "")

            # Change this to your buttons syntax, where {} is the button pressed
            f.write(f'await connection.switch("click {button}")\n')

        elif "Touch" in line:

            # Touch decipher
            x = (line.split("= ", 2)[1]).split(",", 1)[0]
            y = (line.split("= ", 2)[2]).replace("\n", "")

            # Change this to your touch syntax, where {} is the coordinates
            f.write(f'await connection.switch("touch {x} {y}")\n')

        else:
            pass

        # Print sleep/delay time if not 0
        time = (line[6:9]).lstrip("0")

        if time != "" and time.isnumeric() and "..." not in line:

            # Change this to your sleeping syntax
            f.write(f"await asyncio.sleep({time})\n\n")