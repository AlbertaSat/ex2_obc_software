# List of SAT_CLI commands
This is a list of all the sat_cli command help strings as they would be returned from the satellite. This document must maintain parity with help string updates

None of the commands here are guaranteed to work since sat_cli was created as a debugging tool

#### echo:

    Echoes all parameters back

#### hello:

    Says hello :\)

#### time:

    No parameter: get time

    With parameter: set time to parameter

#### imagetype:

    Get type of image booted

#### reboot:

    Reboot to a mode. Can be B, G, or A

#### bootinfo:

    Gives a breakdown of the boot info

#### uptime:

    Get uptime in seconds

#### hostname:

    Returns hostname

#### heap:

    Returns heap stats

#### pwd:

    Get current working directory

#### ls:

    Get list of tiles in cwd

#### cd:

    Change current working directory

#### mkdir:

    Make a new directory

#### rmdir:

    Remove a directory only if it is empty

#### mk:

    Create a new empty file

#### rm:

    Delete file

    Use -r for recursive

#### stat:

    Stat a file

#### read:

    Read contents of a file

#### transact:

    Tell Reliance-edge to transact the filesystem.
    
    Must include volume prefix to transact

#### cp:

    Copy first parameter to second parameter

#### format:

    Format the specified volume. It may take a long time to format and the response may time out, check again later